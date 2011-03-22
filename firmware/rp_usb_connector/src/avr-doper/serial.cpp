/*
 * Name: serial.c
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-07-10
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: serial.c 280 2007-03-20 12:03:11Z cs $
 */

#include "serial.h"
#include "hardware.h"

#if ENABLE_DEBUG_INTERFACE
ringBuffer_t    serialRingBuffer;



ISR_ON_RX_CHAR()   /* runs with global interrupts disabled */
{
    _u8 data = UDR;
    ringBufferWrite(&serialRingBuffer, data);
}


void  ringBufferWrite(ringBuffer_t *rb, _u8 c)
{
    _u8   newWriteIndex = rb->writeIndex + 1;

    if(newWriteIndex >= SERIAL_BUFFER_SIZE)
        newWriteIndex = 0;
    if(newWriteIndex != rb->readIndex){ /* not yet full */
        rb->buffer[rb->writeIndex] = c;
        rb->writeIndex = newWriteIndex;
    }
}

_u8 ringBufferRead(ringBuffer_t *rb)
{
    _u8 c = rb->buffer[rb->readIndex];
    rb->readIndex++;
    if(rb->readIndex >= SERIAL_BUFFER_SIZE)
        rb->readIndex = 0;
    return c;
}

#endif
