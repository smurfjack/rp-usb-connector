/*
 *    ROBOPEAK Project
 * 
 *
 *    Common Includes
 */

#include "conf.h"
#include "arduino_lit.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

//based on stdint.h
typedef int8_t         _s8;
typedef uint8_t        _u8;

typedef int16_t        _s16;
typedef uint16_t       _u16;

typedef int32_t        _s32;
typedef uint32_t       _u32;

typedef int64_t        _s64;
typedef uint64_t       _u64;

// The RDTSC timer value
// Note: In most cases, please use getms() interface to read its value.
//       Except in RF interrupt handler
extern volatile unsigned long timer0_millis; //Real time clock in ms

extern volatile unsigned long timer0_overflow_count;