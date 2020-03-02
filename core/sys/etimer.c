/**
 * \addtogroup etimer
 * @{
 */

/**
 * \file
 * Event timer library implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
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

#include <assert.h>
#include "contiki-conf.h"
#include "sys/etimer.h"
#include "sys/process.h"

#if !defined(CONTIKI_ETIMER_DEBUGPRINTF)
   #define CONTIKI_ETIMER_DEBUGPRINTF(...)
#endif

static struct etimer *timerlist;
static clock_time_t next_expiration;

PROCESS(etimer_process, "Event timer");
/*---------------------------------------------------------------------------*/
static void update_time(void)
/**
 * find the next etimer expiring and initialize the dynamic tick generation accordingly.
 */
{
   assert( !CONTIKI_IN_ISR() );

   if (timerlist == NULL) {
      next_expiration = 0;

      clock_update( clock_time() + MS_TO_CLOCK_SECOND(60000) );    // dummy call to setup a periodic timer interrupt for watchdog triggering
   }
   else {
      next_expiration = timerlist->timer.start + timerlist->timer.interval;

#define xCHECK_LIST
#ifdef CHECK_LIST
      struct etimer *t;
      clock_time_t this_exp;
      clock_time_t next_exp;

      t = timerlist;
      this_exp = t->timer.start + t->timer.interval;
      for (t = t->next; t != NULL; t = t->next) {
         next_exp = t->timer.start + t->timer.interval;

         BTSI_ASSERT( CLOCK_A_GE_B(next_exp, this_exp) );
         this_exp = next_exp;
      }
#endif

      clock_update( next_expiration );
   }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(etimer_process, ev, data)
{
    PROCESS_BEGIN();

    timerlist = NULL;

    for (;;) {
        PROCESS_YIELD();

        if (ev == PROCESS_EVENT_EXITED) {
            struct etimer **t;
            struct process *p = data;

            t = &timerlist;
            while (*t != NULL) {
                if ((*t)->p == p) {
                    *t = (*t)->next;
                }
                else {
                    t = &((*t)->next);
                }
            }
            update_time();
        }
        else if (ev == PROCESS_EVENT_POLL) {
            while (timerlist != NULL  &&  timer_expired( &(timerlist->timer))) {
                struct etimer *t = timerlist;

#if !defined(NDEBUG)
                {
                    // 20ms too late are allowed
                    int32_t delay = (int32_t)(clock_time() - (t->timer.start + t->timer.interval));
                    if (delay > (int32_t)MS_TO_CLOCK_SECOND(20)) {
                        CONTIKI_ETIMER_DEBUGPRINTF( "--> etimer: delayed by %ld ticks in '%s':%d\n",
                                                    delay, t->p->name, t->p->pt.lc );
                    }
                }
#endif

                process_post( t->p, PROCESS_EVENT_TIMER, t );

                // remove timer from list and reset the process id for etimer_expired()
                timerlist = t->next;
                t->p    = PROCESS_NONE;
                t->next = NULL;
            }
            update_time();
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void etimer_request_poll(void)
{
   process_poll(&etimer_process);
}
/*---------------------------------------------------------------------------*/
static void add_timer(struct etimer *timer)
{
   struct etimer **t;
   clock_time_t this_exp;

   etimer_request_poll();

   if (timer->p != PROCESS_NONE) {
      /* Timer on list? */

      for (t = &timerlist; *t != NULL; t = &((*t)->next)) {
         if (*t == timer) {
            /* Timer already on list, temporarily remove it from list. */
#if !defined(NDEBUG)
            if (timer->p != PROCESS_CURRENT()) {
                CONTIKI_ETIMER_DEBUGPRINTF( "--> etimer: etimer gets new owner %s->%s\n", timer->p->name, PROCESS_CURRENT()->name );
            }
#endif
            *t = timer->next;
            break;
         }
      }
   }

   // search the (ordered) list and insert new timer
   this_exp = timer->timer.start + timer->timer.interval;
   for (t = &timerlist; *t != NULL; t = &((*t)->next)) {
       clock_time_t next_exp;

       next_exp = (*t)->timer.start + (*t)->timer.interval;
       if (CLOCK_A_LT_B(this_exp, next_exp)) {
           break;
       }
   }
   timer->next = *t;
   timer->p    = PROCESS_CURRENT();
   *t = timer;

   update_time();
}
/*---------------------------------------------------------------------------*/
void etimer_set(struct etimer *et, clock_time_t interval)
{
   timer_set(&et->timer, interval);
   add_timer(et);
}
/*---------------------------------------------------------------------------*/
void etimer_reset(struct etimer *et)
{
   timer_reset(&et->timer);
   add_timer(et);
}
/*---------------------------------------------------------------------------*/
void etimer_reset_next(struct etimer *et)
{
    if (et->timer.interval == 0) {
        // actually this should not happen
        timer_restart( &et->timer );
    }
    else {
        while (timer_expired(&et->timer)) {
            timer_reset(&et->timer);
        }
    }
    add_timer(et);
}
/*---------------------------------------------------------------------------*/
void etimer_restart(struct etimer *et)
{
   timer_restart(&et->timer);
   add_timer(et);
}
/*---------------------------------------------------------------------------*/
void etimer_adjust(struct etimer *et, int32_t timediff)
{
   //lint -e{737}
   et->timer.start += timediff;
   add_timer(et);
}
/*---------------------------------------------------------------------------*/
int16_t etimer_expired(struct etimer *et)
{
   return et->p == PROCESS_NONE;
}
/*---------------------------------------------------------------------------*/
clock_time_t etimer_expiration_time(struct etimer *et)
{
   return et->timer.start + et->timer.interval;
}
/*---------------------------------------------------------------------------*/
clock_time_t etimer_start_time(struct etimer *et)
{
   return et->timer.start;
}
/*---------------------------------------------------------------------------*/
int16_t etimer_pending(void)
{
   return timerlist != NULL;
}
/*---------------------------------------------------------------------------*/
clock_time_t etimer_next_expiration_time(void)
{
   return etimer_pending() ? next_expiration : 0;
}
/*---------------------------------------------------------------------------*/
void etimer_stop(struct etimer *et)
{
   struct etimer **t;

   for (t = &timerlist; *t != NULL; t = &((*t)->next)) {
      if (*t == et) {
         *t = et->next;
         break;
      }
   }

   /* Remove the next pointer from the item to be removed. */
   et->next = NULL;
   /* Set the timer as expired */
   et->p = PROCESS_NONE;
}
/*---------------------------------------------------------------------------*/
struct etimer *etimer_timerlist( void )
{
   return timerlist;
}
/*---------------------------------------------------------------------------*/
/** @} */
