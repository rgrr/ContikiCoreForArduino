/**
 * \file
 * Default definitions of C compiler quirk work-arounds.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 * This file is used for making use of extra functionality of some C
 * compilers used for Contiki, and defining work-arounds for various
 * quirks and problems with some other C compilers.
 */

/*
 * Copyright (c) 2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the Contiki desktop OS
 */
#ifndef __CC_H__
#define __CC_H__

#include "contiki-conf.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

#define CC_CONCAT2(s1, s2) s1##s2
/**
 * A C preprocessing macro for concatenating to
 * strings.
 *
 * We need use two macros (CC_CONCAT and CC_CONCAT2) in order to allow
 * concatenation of two defined macros.
 */
#define CC_CONCAT(s1, s2) CC_CONCAT2(s1, s2)

#endif /* __CC_H__ */
