/*
 * Name: isp.c
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-21
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: isp.c 566 2008-04-26 14:21:47Z cs $
 */

#include "hardware.h"
#include "utils.h"
#include "isp.h"
#include "timer.h"

static _u8    ispClockDelay;
static _u8    cmdBuffer[4];

/* ------------------------------------------------------------------------- */
/* We disable interrupts while transfer a byte. This ensures that we execute
 * at nominal speed, in spite of aggressive USB polling.
 */
static _u8 ispBlockTransfer(_u8 *block, _u8 len)
{
    _u8   cnt, shift = 0, port, delay = ispClockDelay;

    /* minimum clock pulse width:
     * 5 + 4 * delay clock cycles           -> Tmin = 750 ns
     * total clock period: 12 + 8 * delay   -> fmax = 600 kHz
     */
    cli();
    while(len--){   /* len may be 0 */
        cnt = 8;
        shift = *block++;
        do{
            if(shift & 0x80){
                DIGITAL_WRITE(ISP_MOSI, HIGH);
            }else
            {
                DIGITAL_WRITE(ISP_MOSI, LOW);
            }
            sei();
            timerTicksDelay(delay);
            cli(); 
            DIGITAL_WRITE(ISP_SCK, HIGH);  /* <-- data clocked by device */
            shift <<= 1;

            if(DIGITAL_READ(ISP_MISO))  /* no driver in this hardware */
                shift |= 1;
            sei();
            timerTicksDelay(delay);
            cli();
            DIGITAL_WRITE(ISP_SCK, LOW);    /* <-- device changes data */
        }while(--cnt);
    }
    sei();
    return shift;
}

/* ------------------------------------------------------------------------- */

static void ispAttachToDevice(_u8 stk500Delay, _u8 stabDelay)
{

    if(stk500Delay == 0){ /* 1.8 MHz nominal */
        ispClockDelay = 0;
    }else if(stk500Delay == 1){ /* 460 kHz nominal */
        ispClockDelay = 0;
    }else if(stk500Delay == 2){ /* 115 kHz nominal */
        ispClockDelay = 1;
    }else if(stk500Delay == 3){ /* 58 kHz nominal */
        ispClockDelay = 2;
    }else{
        /* from STK500v2 spec: stk500Delay = 1/(24 * SCK / 7.37 MHz) - 10/12
         * definition of ispClockDelay = 1 + 1/(SCK/MHz * 2 * TIMER_TICK_US)
         * ispClockDelay = 1 + (stk500Delay + 10/12) * 12 / (TIMER_TICK_US * 7.37)
         */
#if F_CPU > 14000000L    /* ~ 16 MHz */
        ispClockDelay = (stk500Delay + 1)/2 - (stk500Delay + 1)/8 + (stk500Delay + 1)/32;
#else   /* must be 12 MHz */
        ispClockDelay = (stk500Delay + 1)/4 + (stk500Delay + 1)/16;
#endif
    }

    LED_TX_ON();
    /* turn on power for programming sockets: */
    PIN_MODE(ISP_SCK, OUTPUT);
    PIN_MODE(ISP_MOSI,OUTPUT);

//    delay(200);   /* allow device to power up */
    PIN_MODE(ISP_RST,OUTPUT);

    delay(stabDelay);
    
    timerTicksDelay(ispClockDelay); /* stabDelay may have been 0 */
    DIGITAL_WRITE(ISP_RST, HIGH);   /* give positive RESET pulse */
    timerTicksDelay(ispClockDelay);
    DIGITAL_WRITE(ISP_RST, LOW);   /* bring RESET back low */
}

static void ispDetachFromDevice()
{
    /* ensure we have no pull-ups active */
    PIN_MODE(ISP_SCK, INPUT);
    PIN_MODE(ISP_MOSI, INPUT);
    PIN_MODE(ISP_RST, INPUT);

    DIGITAL_WRITE(ISP_SCK, LOW);
    DIGITAL_WRITE(ISP_MOSI, LOW);
   
    LED_TX_OFF();
}

/* ------------------------------------------------------------------------- */

_u8   ispEnterProgmode(stkEnterProgIsp_t *param)
{
    _u8   i, rval;
    ispAttachToDevice(stkParam.s.sckDuration, param->stabDelay);
    delay(param->cmdExeDelay);
    /* we want for(i = param->synchLoops; i--;), but avrdude sends synchLoops == 0 */
    for(i = 32; i--;){
        wdt_reset();
        rval = ispBlockTransfer(param->cmd, param->pollIndex);
        if(param->pollIndex < 4)
            ispBlockTransfer(param->cmd + param->pollIndex, 4 - param->pollIndex);
        if(rval == param->pollValue){   /* success: we are in sync */
            return STK_STATUS_CMD_OK;
        }
        /* insert one clock pulse and try again: */
        DIGITAL_WRITE(ISP_SCK, HIGH);
        timerTicksDelay(ispClockDelay);
        DIGITAL_WRITE(ISP_SCK, LOW);
        timerTicksDelay(ispClockDelay);
    }
    ispDetachFromDevice();
    return STK_STATUS_CMD_FAILED;   /* failure */
}

void    ispLeaveProgmode(stkLeaveProgIsp_t *param)
{
    ispDetachFromDevice();
    delay(param->postDelay);
}

/* ------------------------------------------------------------------------- */

static _u8    deviceIsBusy(void)
{
    cmdBuffer[0] = 0xf0;
    cmdBuffer[1] = 0;
    return ispBlockTransfer(cmdBuffer, 4) & 1;
}

// replaced by sleep_alert
static _u8    waitUntilReady(_u8 msTimeout)
{
    timerSetupTimeout(msTimeout);
    while(deviceIsBusy()){
        if(timerTimeoutOccurred())
            return STK_STATUS_RDY_BSY_TOUT;
    }
    return STK_STATUS_CMD_OK;
}

/* ------------------------------------------------------------------------- */

_u8   ispChipErase(stkChipEraseIsp_t *param)
{
    _u8   maxDelay = param->eraseDelay;
    _u8   rval = STK_STATUS_CMD_OK;

    ispBlockTransfer(param->cmd, 4);
    if(param->pollMethod != 0){
        if(maxDelay < 10)   /* allow at least 10 ms */
            maxDelay = 10;
        rval = waitUntilReady(maxDelay);
    }else{
        delay(maxDelay);
    }
    return rval;
}

/* ------------------------------------------------------------------------- */

_u8   ispProgramMemory(stkProgramFlashIsp_t *param, _u8 isEeprom)
{
    utilWord_t  numBytes;
    _u8       rval = STK_STATUS_CMD_OK;
    _u8       valuePollingMask, rdyPollingMask;
    _u16      i;

    numBytes.bytes[1] = param->numBytes[0];
    numBytes.bytes[0] = param->numBytes[1];
    if(param->mode & 1){    /* page mode */
        valuePollingMask = 0x20;
        rdyPollingMask = 0x40;
    }else{                  /* word mode */
        valuePollingMask = 4;
        rdyPollingMask = 8;
    }
    if(!isEeprom && stkAddress.bytes[3] & 0x80){
        cmdBuffer[0] = 0x4d;    /* load extended address */
        cmdBuffer[1] = 0x00;
        cmdBuffer[2] = stkAddress.bytes[2];
        cmdBuffer[3] = 0x00;
        ispBlockTransfer(cmdBuffer, 4);
    }
    
    for(i = 0; rval == STK_STATUS_CMD_OK && i < numBytes.word; i++){
        _u8 x;
        wdt_reset();
        cmdBuffer[1] = stkAddress.bytes[1];
        cmdBuffer[2] = stkAddress.bytes[0];
        cmdBuffer[3] = param->data[i];
        x = param->cmd[0];
        if(!isEeprom){
            x &= ~0x08;
            if((_u8)i & 1){
                x |= 0x08;
                stkIncrementAddress();
            }
        }else{
            stkIncrementAddress();
        }
        cmdBuffer[0] = x;
#if 0   /* does not work this way... */
        if(cmdBuffer[3] == 0xff && !(param->mode & 1) && !isEeprom)   /* skip 0xff in word mode */
            continue;
#endif
        ispBlockTransfer(cmdBuffer, 4);
        if(param->mode & 1){            /* is page mode */
            if(i < numBytes.word - 1 || !(param->mode & 0x80))
                continue;               /* not last byte written */
            cmdBuffer[0] = param->cmd[1];     /* write program memory page */
            ispBlockTransfer(cmdBuffer, 4);
        }
        /* poll for ready after each byte (word mode) or page (page mode) */
        if(param->mode & valuePollingMask){ /* value polling */
            _u8 d = param->data[i];
            if(d == param->poll[0] || d == param->poll[1]){ /* must use timed polling */
                delay(param->delay);
            }else{
                _u8 x = param->cmd[2];     /* read flash */
                x &= ~0x08;
                if((_u8)i & 1){
                    x |= 0x08;
                }
                cmdBuffer[0] = x;
                timerSetupTimeout(param->delay);
                while(ispBlockTransfer(cmdBuffer, 4) != d){
                    if(timerTimeoutOccurred()){
                        rval = STK_STATUS_CMD_TOUT;
                        break;
                    }
                }
            }
        }else if(param->mode & rdyPollingMask){ /* rdy/bsy polling */
            rval = waitUntilReady(param->delay);
        }else{                          /* must be timed delay */
            delay(param->delay);
        }
    }
    return rval;
}

/* ------------------------------------------------------------------------- */

_u16    ispReadMemory(stkReadFlashIsp_t *param, stkReadFlashIspResult_t *result, _u8 isEeprom)
{
    utilWord_t  numBytes;
    _u8       *p, cmd0;
    _u16       i;

    cmdBuffer[3] = 0;
    if(!isEeprom && stkAddress.bytes[3] & 0x80){
        cmdBuffer[0] = 0x4d;    /* load extended address */
        cmdBuffer[1] = 0x00;
        cmdBuffer[2] = stkAddress.bytes[2];
        ispBlockTransfer(cmdBuffer, 4);
    }
    numBytes.bytes[1] = param->numBytes[0];
    numBytes.bytes[0] = param->numBytes[1];
    p = result->data;
    result->status1 = STK_STATUS_CMD_OK;
    cmd0 = param->cmd;
    for(i = 0; i < numBytes.word; i++){
        wdt_reset();
        cmdBuffer[1] = stkAddress.bytes[1];
        cmdBuffer[2] = stkAddress.bytes[0];
        if(!isEeprom){
            if((_u8)i & 1){
                cmd0 |= 0x08;
                stkIncrementAddress();
            }else{
                cmd0 &= ~0x08;
            }
        }else{
            stkIncrementAddress();
        }
        cmdBuffer[0] = cmd0;
        *p++ = ispBlockTransfer(cmdBuffer, 4);
    }
    *p = STK_STATUS_CMD_OK; /* status2 */
    return numBytes.word + 2;
}

/* ------------------------------------------------------------------------- */

_u8   ispProgramFuse(stkProgramFuseIsp_t *param)
{
    ispBlockTransfer(param->cmd, 4);
    return STK_STATUS_CMD_OK;
}

/* ------------------------------------------------------------------------- */

_u8   ispReadFuse(stkReadFuseIsp_t *param)
{
    _u8   rval;

    rval = ispBlockTransfer(param->cmd, param->retAddr);
    if(param->retAddr < 4)
        ispBlockTransfer(param->cmd + param->retAddr, 4 - param->retAddr);
    return rval;
}

/* ------------------------------------------------------------------------- */

_u16    ispMulti(stkMultiIsp_t *param, stkMultiIspResult_t *result)
{
    _u8   cnt1, i, *p;

    cnt1 = param->numTx;
    if(cnt1 > param->rxStartAddr)
        cnt1 = param->rxStartAddr;
    ispBlockTransfer(param->txData, cnt1);

    p = result->rxData;
    for(i = 0; i < param->numTx - cnt1; i++){
        _u8 b = ispBlockTransfer(&param->txData[cnt1] + i, 1);
        if(i < param->numRx)
            *p++ = b;
        wdt_reset();
    }
    
    for(; i < param->numRx; i++){
        cmdBuffer[0] = 0;
        *p++ = ispBlockTransfer(cmdBuffer, 1);
        wdt_reset();
    }
    *p = result->status1 = STK_STATUS_CMD_OK;
    return (_u16)param->numRx + 2;
}

/* ------------------------------------------------------------------------- */
