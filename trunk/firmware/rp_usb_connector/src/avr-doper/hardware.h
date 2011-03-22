/*
 * Name: hardware.h
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-07-05
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: hardware.h 566 2008-04-26 14:21:47Z cs $
 */

/*
General Description:
This module defines hardware properties and configuration choices.
*/

#ifndef __hardware_h_included__
#define __hardware_h_included__

#include "../common.h"


/* Device compatibility: Allow both, ATMega8 and ATMega88/168. The following
 * macros (almost) mimic an ATMega8 based on the ATMega88/168 defines.
 */


#ifdef TCCR0B
#   define TCCR0    TCCR0B
#   define TIMSK    TIMSK0
#endif

#ifdef UBRR0L
#   define UBRRH    UBRR0H
#   define UBRRL    UBRR0L
#   define UCSRA    UCSR0A
#   define UCSRB    UCSR0B
#   define UDR      UDR0
#   define RXEN     RXEN0
#   define TXEN     TXEN0
#   define UDRE     UDRE0
#   define RXCIE    RXCIE0
#endif

#ifdef __AVR_ATmega1280__
#define ISR_ON_RX_CHAR()    \
    ISR(SIG_USART1_RECV)
#elif defined(__AVR_ATmega8__)

#define ISR_ON_RX_CHAR()    \
    ISR(SIG_UART_RECV)
    

#else   //Atmega 48/88/168/328/...

#define ISR_ON_RX_CHAR()    \
    ISR(USART_RX_vect)
#endif

#endif /* __hardware_h_included__ */
