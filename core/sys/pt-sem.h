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

/**
 * \addtogroup pt
 * @{
 */

/**
 * \defgroup ptsem Protothread semaphores
 * @{
 *
 * This module implements counting semaphores on top of
 * protothreads. Semaphores are a synchronization primitive that
 * provide two operations: "wait" and "signal". The "wait" operation
 * checks the semaphore counter and blocks the thread if the counter
 * is zero. The "signal" operation increases the semaphore counter but
 * does not block. If another thread has blocked waiting for the
 * semaphore that is signalled, the blocked thread will become
 * runnable again.
 *
 * Semaphores can be used to implement other, more structured,
 * synchronization primitives such as monitors and message
 * queues/bounded buffers (see below).
 *
 * The following example shows how the producer-consumer problem, also
 * known as the bounded buffer problem, can be solved using
 * protothreads and semaphores. Notes on the program follow after the
 * example.
 *
 \code
#include "pt-sem.h"

#define NUM_ITEMS 32
#define BUFSIZE 8

static struct pt_sem mutex, full, empty;

PT_THREAD(producer(struct pt *pt))
{
  static int produced;

  PT_BEGIN(pt);

  for(produced = 0; produced < NUM_ITEMS; ++produced) {

    PT_SEM_WAIT(pt, &full);

    PT_SEM_WAIT(pt, &mutex);
    add_to_buffer(produce_item());
    PT_SEM_SIGNAL(pt, &mutex);

    PT_SEM_SIGNAL(pt, &empty);
  }

  PT_END(pt);
}

PT_THREAD(consumer(struct pt *pt))
{
  static int consumed;

  PT_BEGIN(pt);

  for(consumed = 0; consumed < NUM_ITEMS; ++consumed) {

    PT_SEM_WAIT(pt, &empty);

    PT_SEM_WAIT(pt, &mutex);
    consume_item(get_from_buffer());
    PT_SEM_SIGNAL(pt, &mutex);

    PT_SEM_SIGNAL(pt, &full);
  }

  PT_END(pt);
}

PT_THREAD(driver_thread(struct pt *pt))
{
  static struct pt pt_producer, pt_consumer;

  PT_BEGIN(pt);

  PT_SEM_INIT(&empty, 0);
  PT_SEM_INIT(&full, BUFSIZE);
  PT_SEM_INIT(&mutex, 1);

  PT_INIT(&pt_producer);
  PT_INIT(&pt_consumer);

  PT_WAIT_THREAD(pt, producer(&pt_producer) &
		     consumer(&pt_consumer));

  PT_END(pt);
}
 \endcode
 *
 * The program uses three protothreads: one protothread that
 * implements the consumer, one thread that implements the producer,
 * and one protothread that drives the two other protothreads. The
 * program uses three semaphores: "full", "empty" and "mutex". The
 * "mutex" semaphore is used to provide mutual exclusion for the
 * buffer, the "empty" semaphore is used to block the consumer is the
 * buffer is empty, and the "full" semaphore is used to block the
 * producer is the buffer is full.
 *
 * The "driver_thread" holds two protothread state variables,
 * "pt_producer" and "pt_consumer". It is important to note that both
 * these variables are declared as <i>static</i>. If the static
 * keyword is not used, both variables are stored on the stack. Since
 * protothreads do not store the stack, these variables may be
 * overwritten during a protothread wait operation. Similarly, both
 * the "consumer" and "producer" protothreads declare their local
 * variables as static, to avoid them being stored on the stack.
 *
 *
 */

/**
 * \file
 * Counting semaphores implemented on protothreads
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 */

#ifndef __PT_SEM_H__
#define __PT_SEM_H__

#include "sys/pt.h"

/**
 * Structure used for keeping the status of a protothread semaphore.
 */
struct pt_sem {
  unsigned int count;
  clock_time_t lastBlock;
  unsigned int unlockWithBroadcast;
  struct process *firstBlocked;
};

#if !defined(PT_SEM_DEBUGPRINTF)
    #define PT_SEM_DEBUGPRINTF(...)
#endif

/**
 * Initialize a semaphore
 *
 * This macro initializes a semaphore with a value for the
 * counter. Internally, the semaphores use an "unsigned int" to
 * represent the counter, and therefore the "count" argument should be
 * within range of an unsigned int.
 *
 * \param s (struct pt_sem *) A pointer to the pt_sem struct
 * representing the semaphore
 *
 * \param c (unsigned int) The initial count of the semaphore.
 * \hideinitializer
 */
#define PT_SEM_INIT(s, c)        \
   do {                          \
      (s)->count = c;            \
   } while (0)

/**
 * Wait for a semaphore
 *
 * This macro carries out the "wait" operation on the semaphore. The
 * wait operation causes the protothread to block while the counter is
 * zero. When the counter reaches a value larger than zero, the
 * protothread will continue.\n
 *
 * \param PT  (struct pt *) A pointer to the protothread (struct pt) in
 *            which the operation is executed.
 *
 * \param SEM (struct pt_sem *) A pointer to the pt_sem struct
 *            representing the semaphore
 *
 * \attention nesting of semaphores is not supported!
 *
 * \hideinitializer
 */
#define PT_SEM_WAIT(PT, SEM)                                                        \
    do {                                                                            \
        if ((SEM)->count > 0) {                                                     \
            PT_SEM_DEBUGPRINTF( "PT_SEM_WAIT(%x): got semaphore for '%s', %d\n", SEM, PROCESS_CURRENT()->name, PROCESS_CURRENT()->pt.lc ); \
            --(SEM)->count;                                                         \
            BTSI_ASSERT( PROCESS_CURRENT()->sem_owning == NULL );                   \
            PROCESS_CURRENT()->sem_owning = SEM;                                    \
            (SEM)->lastBlock = clock_time();                                        \
            break;                                                                  \
        }                                                                           \
        PT_SEM_DEBUGPRINTF( "PT_SEM_WAIT(%x): blocking '%s', %d\n", SEM, PROCESS_CURRENT()->name, PROCESS_CURRENT()->pt.lc ); \
        if ((SEM)->firstBlocked == NULL) {                                          \
            (SEM)->firstBlocked = PROCESS_CURRENT();                                \
        }                                                                           \
        else {                                                                      \
            (SEM)->unlockWithBroadcast = 1;                                         \
        }                                                                           \
        PT_YIELD( PT );                                                             \
        BTSI_ASSERT( clock_time() - (SEM)->lastBlock < 1800*CLOCK_SECOND );         \
    } while (1)


/**
 * Signal a semaphore
 *
 * This macro carries out the "signal" operation on the semaphore. The
 * signal operation increments the counter inside the semaphore and sends
 * the signal PROCESS_EVENT_SEMSIGNAL if required.
 *
 * \param PT  (struct pt *) A pointer to the protothread (struct pt) in
 *            which the operation is executed.
 *
 * \param SEM (struct pt_sem *) A pointer to the pt_sem struct
 *            representing the semaphore
 *
 * \hideinitializer
 */
#define PT_SEM_SIGNAL(PT, SEM)                                                  \
    do {                                                                        \
        bool pause = false;                                                     \
        ++(SEM)->count;                                                         \
        PROCESS_CURRENT()->sem_owning = NULL;                                   \
        if ((SEM)->firstBlocked != NULL) {                                      \
            PT_SEM_DEBUGPRINTF( "PT_SEM_SIGNAL(%x) released semaphore for '%s', %d SINGLE unblocking, try '%s'\n", SEM, PROCESS_CURRENT()->name, PROCESS_CURRENT()->pt.lc, (SEM)->firstBlocked->name ); \
            process_post( (SEM)->firstBlocked, PROCESS_EVENT_SEMSIGNAL, SEM );  \
            (SEM)->firstBlocked = NULL;                                         \
            pause = true;                                                       \
        }                                                                       \
        else if ((SEM)->unlockWithBroadcast) {                                  \
            PT_SEM_DEBUGPRINTF( "PT_SEM_SIGNAL(%x) released semaphore for '%s', %d BROADCAST unblock\n", SEM, PROCESS_CURRENT()->name, PROCESS_CURRENT()->pt.lc ); \
            process_post( PROCESS_BROADCAST, PROCESS_EVENT_SEMSIGNAL, SEM );    \
            (SEM)->unlockWithBroadcast = 0;                                     \
            pause = true;                                                       \
        }                                                                       \
        else {                                                                  \
            PT_SEM_DEBUGPRINTF( "PT_SEM_SIGNAL(%x) released semaphore for '%s', %d NOBODY to unblock\n", SEM, PROCESS_CURRENT()->name, PROCESS_CURRENT()->pt.lc ); \
        }                                                                       \
        if (pause) {                                                            \
            process_post( PROCESS_CURRENT(), PROCESS_EVENT_SEMSIGNAL, SEM );    \
            PT_YIELD( PT );                                                     \
        }                                                                       \
    } while (0)

#endif /* __PT_SEM_H__ */

/** @} */
/** @} */

