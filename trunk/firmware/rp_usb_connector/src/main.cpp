/*
 *  RoboPeak Bridge 
 *  By Shikai Chen
 *
 */

/*
   Copyright statements of the reference source:
*/
/* Name: main.c
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-06-21
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: main.c 702 2008-11-26 20:16:54Z cs $
 */

/*
General Description:
This module implements hardware initialization and the USB interface
*/

#include "common.h"

extern "C"{
#include "usbdrv/usbdrv.h"
}

#include "avr-doper/utils.h"
#include "stk500protocol.h"
#include "avr-doper/serial.h"

#include "working_mode.h"

volatile _u8    bridgeWorkingMode;

static void setWorkingType(void)
{
    // retrieve the working mode from the current P_PSPD pin
//	if (DIGITAL_READ(JUMPER_MODE_SEL)) {
		bridgeWorkingMode = MODE_AVR_DOPER;
        LED_TX_ON();
        delay(100);
        LED_TX_OFF();
    
        delay(100);
      
        LED_RX_ON();
        delay(100);
        LED_RX_OFF();
//	} else {
//		bridgeWorkingMode = MODE_RF_AP;
//		init_rf_dev();
//	}
}

void setup(void)
{
    init();
    PORTB = 0;
    DDRB = 0;
    PORTC = 0;
    DDRC = 0;
    PORTD = 0;
    DDRD = 0;

    LED_INIT();

    DIGITAL_WRITE(JUMPER_MODE_SEL, HIGH);
    
    cbi(USB_PORT, USB_PORT_P);
    cbi(USB_PORT, USB_PORT_N);
    cbi(USB_DDR, USB_PORT_P);
    cbi(USB_DDR, USB_PORT_N);



    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
}

static inline void postsetup(void)
{
    // Connect the USB
	wdt_reset();
	delay(128);
	delay(128);
	usbDeviceConnect();
}


int main(void)
{
    wdt_enable(WDTO_2S);

#if ENABLE_DEBUG_INTERFACE
    serialInit();
#endif

    setup();
    setWorkingType();
    postsetup();
    usbInit();
    sei();
    for(;;){    /* main event loop */
        wdt_reset();
        usbPoll();
        stkPoll();
    }
    
    return 0;
}
