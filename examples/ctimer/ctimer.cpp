#include <Arduino.h>
#include "contiki.h"

PROCESS( DemoOutput, "DemoOutput" );



static void ctimer_callback( void *para )
{
    Serial.print( "      in ctimer_callback, time is " );
    Serial.print( CLOCK_SECOND_TO_MS(clock_time()), 10 );
    Serial.println( "[ms]" );
}   // ctimer_callback



PROCESS_THREAD( DemoOutput, ev, data )
/**
 * Every second output the current contiki time.
 */
{
    static struct etimer timer;  // beware: process variables must be static, see contiki documentation
    static struct ctimer ctimer;

    PROCESS_BEGIN();

    etimer_set( &timer, MS_TO_CLOCK_SECOND( 500 ) );
    PROCESS_WAIT_UNTIL( etimer_expired( &timer) );

    Serial.println( "Starting DemoOutput()" );
    PROCESS_PAUSE();

    etimer_set( &timer, MS_TO_CLOCK_SECOND( 5000 ) );
    for (;;) {
        PROCESS_WAIT_UNTIL( etimer_expired( &timer) );

        etimer_reset( &timer );
        ctimer_set( &ctimer, MS_TO_CLOCK_SECOND(100), ctimer_callback, NULL );

        Serial.print( "contiki time: " );
        Serial.print( clock_time(), 10 );
        Serial.print( "[tt] " );
        Serial.print( CLOCK_SECOND_TO_MS(clock_time()), 10 );
        Serial.println( "[ms]" );

        Serial.println( "   ctimer started with 100ms" );
    }

    PROCESS_END();
}   // PROCESS_THREAD( DemoOutput )



void setup() 
{
    Serial.begin(115200);

    // low level contiki startup
    clock_start();
    process_init();
    ctimer_init();

    // start contiki processes.  If you want to use etimer, it must be started here
    process_start( &etimer_process, NULL );
    process_start( &DemoOutput, NULL );
}   // setup



void loop() 
{
    //
    // this is the basic contiki scheduler:
    // - poll etimer process so that it can check expired etimers
    // - run waiting processes
    //
    process_poll( &etimer_process );
    for (;;) {
        if (process_run() == 0) {
            break;
        }
    }

    delay( 10 );
}   // loop
