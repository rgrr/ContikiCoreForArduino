#if defined(ARDUINO_ARCH_RP2040)

#include <hardware/timer.h>
#include "contiki.h"



/**
 * Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in system ticks.
 */
clock_time_t clock_time(void)
{
    uint64_t time_us = time_us_64();
    return (clock_time_t)((CLOCK_SECOND * time_us) / 1000000);
}   // clock_time



/**
 * Initialize the interrupt system for the next etimer event.
 */
void clock_update( clock_time_t next_event )
{
    // TODO actual implementation
}   // clock_update



void clock_start( void )
{
}   // clock_start

#endif
