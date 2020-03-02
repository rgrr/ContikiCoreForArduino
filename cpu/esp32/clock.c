#include "contiki.h"

#include <esp32-hal-timer.h>


/* create a hardware timer */
hw_timer_t * timer = NULL;




/**
 * Get the current clock time.
 *
 * This function returns the current system clock time.
 *
 * \return The current clock time, measured in system ticks.
 */
clock_time_t clock_time(void)
{
    return timerRead( timer );
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
    timer = timerBegin( 0, 80000000 / CLOCK_CONF_SECOND, true );
}   // clock_start
