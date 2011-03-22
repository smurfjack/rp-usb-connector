/*
 *  RoboPeak Bridge 
 *  By Shikai Chen
 *
 */


/*
 * Name: stk500protocol.c
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-19
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: stk500protocol.c 566 2008-04-26 14:21:47Z cs $
 */

#include "avr-doper/hardware.h"
#include "stk500protocol.h"
#include "avr-doper/utils.h"
#include "avr-doper/isp.h"
#include "avr-doper/timer.h"


/* The following versions are reported to the programming software: */
#define STK_VERSION_HW      1
#define STK_VERSION_MAJOR   2
#define STK_VERSION_MINOR   4

#define BUFFER_SIZE     281 /* results in 275 bytes max body size */
#define RX_TIMEOUT      200 /* timeout in milliseconds */

#define STK_TXMSG_START 5


static _u8        rxBuffer[BUFFER_SIZE];
static _u16       rxPos;
static utilWord_t rxLen;
static _u8        rxBlockAvailable;

static _u8        txBuffer[BUFFER_SIZE];
static _u16       txPos, txLen;

stkParam_t      stkParam = {{
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    STK_VERSION_HW, STK_VERSION_MAJOR, STK_VERSION_MINOR, 0, 50, 0, 1, 0x80,
                    2, 0, 0xaa, 0, 0, 0, 0, 0,
                }};
utilDword_t     stkAddress;

/* ------------------------------------------------------------------------- */

void    stkIncrementAddress(void)
{
    stkAddress.dword++;
}

/* ------------------------------------------------------------------------- */

void stkSetTxMessage(_u16 len)
{
    _u8   *p = txBuffer, sum = 0;

    *p++ = STK_STX;
    *p++ = rxBuffer[1];  /* sequence number */
    *p++ = utilHi8(len);
    *p++ = len;
    *p++ = STK_TOKEN;
    txPos = 0;
    len += 6;
    txLen = len--;
    p = txBuffer;
    while(len--){
        sum ^= *p++;
    }
    *p = sum;
}

/* ------------------------------------------------------------------------- */

static void setParameter(_u8 index, _u8 value)
{
    if(index == STK_PARAM_OSC_PSCALE){
        // NOT SUPPORT
    }else if(index == STK_PARAM_OSC_CMATCH){
        // OCR2 = value;
        // TODO!
    }
    index &= 0x1f;
    stkParam.bytes[index] = value;
}

static _u8 getParameter(_u8 index)
{
    if(index == STK_PARAM_OSC_PSCALE)
        // NOT SUPPORT
        return 0;//HW_GET_T2_PRESCALER();
    if(index == STK_PARAM_OSC_CMATCH)
        // NOT SUPPORT
        return 0;//OCR2;
    index &= 0x1f;
    return stkParam.bytes[index];
}

/* ------------------------------------------------------------------------- */

/* Use defines for the switch statement so that we can choose between an
 * if()else if() and a switch/case based implementation. switch() is more
 * efficient for a LARGE set of sequential choices, if() is better in all other
 * cases.
 */
#if 0
#define SWITCH_START        if(0){
#define SWITCH_CASE(value)  }else if(cmd == (value)){
#define SWITCH_CASE2(v1,v2) }else if(cmd == (v1) || cmd == (v2)){
#define SWITCH_CASE3(v1,v2,v3) }else if(cmd == (v1) || cmd == (v2) || (cmd == v3)){
#define SWITCH_CASE4(v1,v2,v3,v4) }else if(cmd == (v1) || cmd == (v2) || cmd == (v3) || cmd == (v4)){
#define SWITCH_DEFAULT      }else{
#define SWITCH_END          }
#else
#define SWITCH_START        switch(cmd){{
#define SWITCH_CASE(value)  }break; case (value):{
#define SWITCH_CASE2(v1,v2) }break; case (v1): case(v2):{
#define SWITCH_CASE3(v1,v2,v3) }break; case (v1): case(v2): case(v3):{
#define SWITCH_CASE4(v1,v2,v3,v4) }break; case (v1): case(v2): case(v3): case(v4):{
#define SWITCH_DEFAULT      }break; default:{
#define SWITCH_END          }}
#endif

void stkEvaluateRxMessage(void) /* not static to prevent inlining */
{
    _u8       i, cmd;
    utilWord_t  len = {2};  /* defaults to cmd + error code */
    void        *param;

    cmd = rxBuffer[STK_TXMSG_START];
    txBuffer[STK_TXMSG_START] = cmd;
    txBuffer[STK_TXMSG_START + 1] = STK_STATUS_CMD_OK;
    param = &rxBuffer[STK_TXMSG_START + 1];
    //
#if 0
    SWITCH_START
    SWITCH_CASE(STK_CMD_SIGN_ON)
        static PROGMEM _u8 string[] = {8, 'S', 'T', 'K', '5', '0', '0', '_', '2', 0};
        _u8 *p = &txBuffer[STK_TXMSG_START + 2];
        strcpy_P((char*)p, (PGM_P)string);
        len.bytes[0] = 11;
    SWITCH_CASE(STK_CMD_SET_PARAMETER)
        setParameter(rxBuffer[STK_TXMSG_START + 1], rxBuffer[STK_TXMSG_START + 2]);
    SWITCH_CASE(STK_CMD_GET_PARAMETER)
        txBuffer[STK_TXMSG_START + 2] = getParameter(rxBuffer[STK_TXMSG_START + 1]);
        len.bytes[0] = 3;
    SWITCH_CASE(STK_CMD_OSCCAL)
        txBuffer[STK_TXMSG_START + 1] = STK_STATUS_CMD_FAILED;
        /* not yet implemented */
    SWITCH_CASE(STK_CMD_LOAD_ADDRESS)
        for(i=0;i<4;i++){
            stkAddress.bytes[3-i] = rxBuffer[STK_TXMSG_START + 1 + i];
        }
    SWITCH_CASE(STK_CMD_SET_CONTROL_STACK)
        /* AVR Studio sends:
        1b 08 00 21 0e 2d 
        4c 0c 1c 2c 3c 64 74 66
        68 78 68 68 7a 6a 68 78
        78 7d 6d 0c 80 40 20 10
        11 08 04 02 03 08 04 00
        bf
        */
        /* dummy: ignore */

    SWITCH_CASE(STK_CMD_ENTER_PROGMODE_ISP)
        txBuffer[STK_TXMSG_START + 1] = ispEnterProgmode((stkEnterProgIsp_t *)param);
    SWITCH_CASE(STK_CMD_LEAVE_PROGMODE_ISP)
        ispLeaveProgmode((stkLeaveProgIsp_t *)param);
    SWITCH_CASE(STK_CMD_CHIP_ERASE_ISP)
        txBuffer[STK_TXMSG_START + 1] = ispChipErase((stkChipEraseIsp_t *)param);
    SWITCH_CASE2(STK_CMD_PROGRAM_FLASH_ISP, STK_CMD_PROGRAM_EEPROM_ISP)
        txBuffer[STK_TXMSG_START + 1] = ispProgramMemory((stkProgramFlashIsp_t *)param, cmd-STK_CMD_PROGRAM_FLASH_ISP); 
    SWITCH_CASE2(STK_CMD_READ_FLASH_ISP, STK_CMD_READ_EEPROM_ISP)
        len.word = 1 + ispReadMemory((stkReadFlashIsp_t *)param, (stkReadFlashIspResult_t *)&txBuffer[STK_TXMSG_START + 1], cmd-STK_CMD_READ_FLASH_ISP);
    SWITCH_CASE(STK_CMD_PROGRAM_FUSE_ISP)
        txBuffer[STK_TXMSG_START + 1] = ispProgramFuse((stkProgramFuseIsp_t *)param);
    SWITCH_CASE4(STK_CMD_READ_FUSE_ISP, STK_CMD_READ_LOCK_ISP, STK_CMD_READ_SIGNATURE_ISP, STK_CMD_READ_OSCCAL_ISP)
        txBuffer[STK_TXMSG_START + 2] = ispReadFuse((stkReadFuseIsp_t *)param);
        txBuffer[STK_TXMSG_START + 3] = STK_STATUS_CMD_OK;
        len.bytes[0] = 4;
    SWITCH_CASE(STK_CMD_PROGRAM_LOCK_ISP)
        txBuffer[STK_TXMSG_START + 1] = ispProgramFuse((stkProgramFuseIsp_t *)param);
    SWITCH_CASE(STK_CMD_SPI_MULTI)
        len.word = 1 + ispMulti((stkMultiIsp_t *)param, (stkMultiIspResult_t *)&txBuffer[STK_TXMSG_START + 1]);
    SWITCH_DEFAULT  /* unknown command */
        txBuffer[STK_TXMSG_START + 1] = STK_STATUS_CMD_FAILED;
    SWITCH_END
#else
    // reduce the code size by removing the switch-case jump table
    LED_RX_ON();
    if (cmd == STK_CMD_ENTER_PROGMODE_ISP)
    {
        txBuffer[STK_TXMSG_START + 1] = ispEnterProgmode((stkEnterProgIsp_t *)param);
    }else if (cmd == STK_CMD_LEAVE_PROGMODE_ISP)
    {
        ispLeaveProgmode((stkLeaveProgIsp_t *)param);
    }else if (cmd == STK_CMD_CHIP_ERASE_ISP)
    {
        txBuffer[STK_TXMSG_START + 1] = ispChipErase((stkChipEraseIsp_t *)param);
    }else if (cmd == STK_CMD_PROGRAM_FLASH_ISP || cmd == STK_CMD_PROGRAM_EEPROM_ISP)
    {
        txBuffer[STK_TXMSG_START + 1] = ispProgramMemory((stkProgramFlashIsp_t *)param, cmd-STK_CMD_PROGRAM_FLASH_ISP); 
    }else if (cmd == STK_CMD_READ_FLASH_ISP || cmd == STK_CMD_READ_EEPROM_ISP)
    {
        len.word = 1 + ispReadMemory((stkReadFlashIsp_t *)param, (stkReadFlashIspResult_t *)&txBuffer[STK_TXMSG_START + 1], cmd-STK_CMD_READ_FLASH_ISP);
    }else if (cmd == STK_CMD_PROGRAM_FUSE_ISP)
    {
        txBuffer[STK_TXMSG_START + 1] = ispProgramFuse((stkProgramFuseIsp_t *)param);
    }else if (cmd == STK_CMD_READ_FUSE_ISP || cmd == STK_CMD_READ_LOCK_ISP || cmd == STK_CMD_READ_SIGNATURE_ISP || cmd == STK_CMD_READ_OSCCAL_ISP)
    {
        txBuffer[STK_TXMSG_START + 2] = ispReadFuse((stkReadFuseIsp_t *)param);
        txBuffer[STK_TXMSG_START + 3] = STK_STATUS_CMD_OK;
        len.bytes[0] = 4;
    }else if (cmd == STK_CMD_PROGRAM_LOCK_ISP)
    {
        txBuffer[STK_TXMSG_START + 1] = ispProgramFuse((stkProgramFuseIsp_t *)param);
    }else if (cmd == STK_CMD_SPI_MULTI)
    {
        len.word = 1 + ispMulti((stkMultiIsp_t *)param, (stkMultiIspResult_t *)&txBuffer[STK_TXMSG_START + 1]);
    }else if (cmd == STK_CMD_SIGN_ON)
    {
        static PROGMEM _u8 string[] = {8, 'S', 'T', 'K', '5', '0', '0', '_', '2', 0};
        _u8 *p = &txBuffer[STK_TXMSG_START + 2];
        strcpy_P((char*)p, (PGM_P)string);
        len.bytes[0] = 11;
    }else if (cmd == STK_CMD_SET_PARAMETER)
    {
        setParameter(rxBuffer[STK_TXMSG_START + 1], rxBuffer[STK_TXMSG_START + 2]);
    }else if (cmd == STK_CMD_GET_PARAMETER)
    {
        txBuffer[STK_TXMSG_START + 2] = getParameter(rxBuffer[STK_TXMSG_START + 1]);
        len.bytes[0] = 3;
    }else if (cmd == STK_CMD_LOAD_ADDRESS)
    {
        for(i=0;i<4;i++){
            stkAddress.bytes[3-i] = rxBuffer[STK_TXMSG_START + 1 + i];
        }
    }else if (cmd == STK_CMD_SET_CONTROL_STACK)
    {
        // ignore
    }
       
    else{
        txBuffer[STK_TXMSG_START + 1] = STK_STATUS_CMD_FAILED;
    }
    
    LED_RX_OFF();
#endif
    //    
    stkSetTxMessage(len.word);
}

/* ------------------------------------------------------------------------- */

void    stkSetRxChar(_u8 c)
{
    static _u8 s_sum = 0;
    
    if(timerLongTimeoutOccurred()){
        rxPos = 0;
    }
    if(rxPos == 0){     /* only accept STX as the first character */
        if(c == STK_STX){
            s_sum = 0;
        }else{
            goto FINAL;
        }
    }
    
        if(rxPos < BUFFER_SIZE){
            rxBuffer[rxPos++] = c;
            s_sum ^= c;
            if(rxPos == 4){     /* do we have length byte? */
                rxLen.bytes[0] = rxBuffer[3];
                rxLen.bytes[1] = rxBuffer[2];
                rxLen.word += 6;
                if(rxLen.word > BUFFER_SIZE){    /* illegal length */
                    rxPos = 0;      /* reset state */
                }
            }else if(rxPos == 5){   /* check whether this is the token byte */
                if(c != STK_TOKEN){
                    rxPos = 0;  /* reset state */
                }
            }else if(rxPos > 4 && rxPos == rxLen.word){ /* message is complete */
                rxPos = 0;
                if(s_sum == 0){   /* check sum is correct, evaluate rx message */
                    rxBlockAvailable = 1;
                }else{          /* checksum error */
                    txBuffer[STK_TXMSG_START] = STK_ANSWER_CKSUM_ERROR;
                    txBuffer[STK_TXMSG_START + 1] = STK_ANSWER_CKSUM_ERROR;
                    stkSetTxMessage(2);
                }
            }
        }else{  /* overflow */
            rxPos = 0;  /* reset state */
        }


FINAL:
    timerSetupLongTimeout(RX_TIMEOUT);
}

int stkGetTxCount(void)
{
    return txLen - txPos;
}

int stkGetTxByte(void)
{
    _u8   c;

    if(txLen == 0){
        return -1;
    }
    c = txBuffer[txPos++];
    if(txPos >= txLen){         /* transmit ready */
        txPos = txLen = 0;
    }
    return c;
}

void    stkPoll(void)
{
    if(rxBlockAvailable){
        rxBlockAvailable = 0;
        stkEvaluateRxMessage();
    }
}

/* ------------------------------------------------------------------------- */
