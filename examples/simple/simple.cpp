#include <Arduino.h>
#include "contiki.h"

PROCESS( DemoOutput, "DemoOutput" );



PROCESS_THREAD( DemoOutput, ev, data )
/**
 * Every second output the current contiki time.
 */
{
    static etimer timer;  // beware: process variables must be static, see contiki documentation

    PROCESS_BEGIN();

    Serial.println( "Starting DemoOutput()" );
    PROCESS_PAUSE();

    etimer_set( &timer, MS_TO_CLOCK_SECOND( 1000 ) );
    for (;;) {
        Serial.print( "contiki time: " );
        Serial.print( clock_time(), 10 );
        Serial.print( "[tt] " );
        Serial.print( CLOCK_SECOND_TO_MS(clock_time()), 10 );
        Serial.println( "[ms]" );

        PROCESS_WAIT_UNTIL( etimer_expired( &timer) );
        etimer_reset( &timer );
    }

    PROCESS_END();
}   // PROCESS_THREAD( DemoOutput )



void setup() 
{
    Serial.begin(115200);

    // low level contiki startup
    clock_start();
    process_init();

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
