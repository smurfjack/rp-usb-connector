/*
 * Name: utils.h
 * Project: AVR-Doper
 * Author: Christian Starkjohann <cs@obdev.at>
 * Creation Date: 2006-07-02
 * Tabsize: 4
 * Copyright: (c) 2006 by Christian Starkjohann, all rights reserved.
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Revision: $Id: utils.h 566 2008-04-26 14:21:47Z cs $
 */

/*
General Description:
This module contains general purpose macros and functions:
  - uchar and uint data types
  - highbyte/lowbyte access
  - string concatenation tricks
  - binary constant interpretation
  - port pin access
*/

#ifndef __utils_h_included__
#define __utils_h_included__


#ifndef __ASSEMBLER__

typedef union{
    _u16  word;
    _u8   bytes[2];
}utilWord_t;

typedef union{
    _u32          dword;
    _u8           bytes[4];
}utilDword_t;

/* ------------------------------------------------------------------------- */

static inline _u8 utilHi8(_u16 x)
{
    utilWord_t converter;

    converter.word = x;
    return converter.bytes[1];
}

/* ------------------------------------------------------------------------- */

#endif  /* !defined(__ASSEMBLER__) */

#define UTIL_CONCAT(a, b)           a ## b
#define UTIL_CONCAT_EXPANDED(a, b)  UTIL_CONCAT(a, b)

/*
#define UTIL_INTERRUPT(signame)                         \
    void signame (void) __attribute__ ((interrupt));    \
    void signame (void)
*/
/* ------------------------------------------------------------------------- */

/* PORT bit macros */

#define UTIL_ARG1(a, b) a
#define UTIL_ARG2(a, b) b

#define UTIL_PBIT_SET(varbase, pinspec)  UTIL_CONCAT_EXPANDED(varbase, UTIL_ARG1(pinspec)) |= (1 << (UTIL_ARG2(pinspec)))
#define UTIL_PBIT_CLR(varbase, pinspec)  UTIL_CONCAT_EXPANDED(varbase, UTIL_ARG1(pinspec)) &= ~(1 << (UTIL_ARG2(pinspec)))

#define PORT_OUT(pinspec)       UTIL_CONCAT_EXPANDED(PORT, UTIL_ARG1(pinspec))
#define PORT_IN(pinspec)        UTIL_CONCAT_EXPANDED(PIN, UTIL_ARG1(pinspec))
#define PORT_DDR(pinspec)       UTIL_CONCAT_EXPANDED(DDR, UTIL_ARG1(pinspec))
#define PORT_BIT(pinspec)       UTIL_ARG2(pinspec)

#define PORT_PIN_SET(pinspec)   UTIL_CONCAT_EXPANDED(PORT, UTIL_ARG1(pinspec)) |= (1 << (UTIL_ARG2(pinspec)))
#define PORT_PIN_CLR(pinspec)   UTIL_CONCAT_EXPANDED(PORT, UTIL_ARG1(pinspec)) &= ~(1 << (UTIL_ARG2(pinspec)))
#define PORT_PIN_VALUE(pinspec) (UTIL_CONCAT_EXPANDED(PIN, UTIL_ARG1(pinspec)) & (1 << (UTIL_ARG2(pinspec))))

#define PORT_DDR_SET(pinspec)   UTIL_CONCAT_EXPANDED(DDR, UTIL_ARG1(pinspec)) |= (1 << (UTIL_ARG2(pinspec)))
#define PORT_DDR_CLR(pinspec)   UTIL_CONCAT_EXPANDED(DDR, UTIL_ARG1(pinspec)) &= ~(1 << (UTIL_ARG2(pinspec)))
#define PORT_DDR_VALUE(pinspec) (UTIL_CONCAT_EXPANDED(DDR, UTIL_ARG1(pinspec)) & (1 << (UTIL_ARG2(pinspec))))

/* ------------------------------------------------------------------------- */

#endif /* __utils_h_included__ */
