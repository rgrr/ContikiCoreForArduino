/** \addtogroup sys
 * @{
 */

/**
 * \defgroup clock Clock library
 *
 * The clock library is the interface between Contiki and the platform
 * specific clock functionality. The clock library performs a single
 * function: measuring time. Additionally, the clock library provides
 * a macro, CLOCK_SECOND, which corresponds to one second of system
 * time.
 *
 * \note The clock library need in many cases not be used
 * directly. Rather, the \ref timer "timer library" or the \ref etimer
 * "event timers" should be used.
 *
 * \sa \ref timer "Timer library"
 * \sa \ref etimer "Event timers"
 *
 * @{
 */

/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 */
#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "contiki-conf.h"

#ifdef __cplusplus
    extern "C"
    {
#endif //__cplusplus

/**
 * Check if a clock time value is less than another clock time value.
 *
 * This macro checks if a clock time value is less than another clock
 * time value. This macro is needed to correctly handle wrap-around of
 * clock time values.
 *
 * \retval true if a < b
 *
 */
#define CLOCK_A_LT_B(a, b) ((int32_t)((a) - (b)) < 0)

/**
 * Check if a clock time value is greater or equal than another clock time value.
 *
 * This macro checks if a clock time value is greater or equal than another clock
 * time value. This macro is needed to correctly handle wrap-around of
 * clock time values.
 *
 * \retval true if a >= b
 *
 */
#define CLOCK_A_GE_B(a, b) ((int32_t)((a) - (b)) >= 0)

/**
 * Maximum delta clock time which can be recognized by \ref CLOCK_A_LT_B() and \ref CLOCK_A_GE_B()
 */
#define CLOCK_MAX_DELTA  0x7fffffff

/**
 * Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in system ticks.
 */
clock_time_t clock_time(void);

/**
 * Initialize the interrupt system for the next etimer event.
 */
void clock_update( clock_time_t next_event );

/**
 * A second, measured in system clock time.
 *
 * \hideinitializer
 */
#if defined(CLOCK_CONF_SECOND)  ||  defined(DOXYGEN)
#define CLOCK_SECOND CLOCK_CONF_SECOND
#else
// no default value here!
#endif

/** Convert milliseconds to clock ticks (integer version).  Range 0..500000ms */
#define MS_TO_CLOCK_SECOND(MS)     ((clock_time_t)(((MS) * CLOCK_SECOND + 1000/2) / 1000))

/** Convert milliseconds to clock ticks (floating point version) */
#define MS_TO_CLOCK_SECOND_F(MS)   ((clock_time_t)(((float)(MS) * (float)CLOCK_SECOND) / 1000.0 + 0.5))

/** Convert seconds to clock ticks (integer version).  Range 0..500000s */
#define SEC_TO_CLOCK_SECOND(SEC)   ((clock_time_t)((SEC) * CLOCK_SECOND))

/** Convert minutes to clock ticks (integer version).  Range 0..8500min */
#define MIN_TO_CLOCK_SECOND(MIN)   ((clock_time_t)(60UL * (MIN) * CLOCK_SECOND))

/** Convert clock ticks to milliseconds.  Beware of overflow after 4 million ticks! */
#define CLOCK_SECOND_TO_MS(TICKS)  ((uint32_t)(((TICKS) * 1000 + CLOCK_SECOND/2) / CLOCK_SECOND))

#ifdef __cplusplus
    }
#endif //__cplusplus

#endif /* __CLOCK_H__ */

/** @} */
/** @} */
