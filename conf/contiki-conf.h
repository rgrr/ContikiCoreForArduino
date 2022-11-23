/* -*- C -*- */

#ifndef CONTIKI_CONF_H
#define CONTIKI_CONF_H

#include <stdint.h>

#define PROCESS_CONF_NUMEVENTS 32
#define PROCESS_CONF_STATS     1

/** 32768/32 [Hz] */
#define CLOCK_CONF_SECOND      1000

#define CONTIKI_PRINTF(...)

/**
 * Type for time stamps.
 * This must be 32bit or you have to change the macros \ref CLOCK_A_LT_B()
 * and \ref CLOCK_A_GE_B().
 */
typedef uint32_t clock_time_t;

#if 0
    #define CONTIKI_IN_ISR()       ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)
#else
    #define CONTIKI_IN_ISR()       0
#endif



#ifdef __cplusplus
    extern "C"
    {
#endif //__cplusplus

void clock_start( void );

#ifdef __cplusplus
    }
#endif //__cplusplus

#endif /* CONTIKI_CONF_H */
