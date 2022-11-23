#if defined(ARDUINO_ARCH_RP2040)

#include "contiki.h"



static clock_time_t mytime;



/**
 * Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in system ticks.
 */
clock_time_t clock_time(void)
{
    return ++mytime;
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
    mytime = 0;
}   // clock_start

#endif
