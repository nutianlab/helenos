/*
 * Copyright (C) 2005 Jakub Jermar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __amd64_ASM_H__
#define __amd64_ASM_H__

#include <arch/types.h>
#include <config.h>


void asm_delay_loop(__u32 t);
void asm_fake_loop(__u32 t);


/* TODO: implement the real stuff */
static inline __address get_stack_base(void)
{
	return NULL;
}

static inline void cpu_sleep(void) { __asm__("hlt"); };


static inline __u8 inb(__u16 port) 
{
	__u8 out;

	asm (
		"mov %0, %%dx;"
		"inb %%dx,%%al;"
		"mov %%al, %1;"
		:"=m"(out)
		:"m"(port)
		:"%dx","%al"
		);
	return out;
}

static inline __u8 outb(__u16 port,__u8 b) 
{
	asm (
		"mov %0,%%dx;"
		"mov %1,%%al;"
		"outb %%al,%%dx;"
		:
		:"m"( port), "m" (b)
		:"%dx","%al"
		);
}

/** Set priority level low
 *
 * Enable interrupts and return previous
 * value of EFLAGS.
 */
static inline pri_t cpu_priority_low(void) {
	pri_t v;
	__asm__ volatile (
		"pushfq\n"
		"popq %0\n"
		"sti\n"
		: "=r" (v)
	);
	return v;
}

/** Set priority level high
 *
 * Disable interrupts and return previous
 * value of EFLAGS.
 */
static inline pri_t cpu_priority_high(void) {
	pri_t v;
	__asm__ volatile (
		"pushfq\n"
		"popq %0\n"
		"cli\n"
		: "=r" (v)
		);
	return v;
}

/** Restore priority level
 *
 * Restore EFLAGS.
 */
static inline void cpu_priority_restore(pri_t pri) {
	__asm__ volatile (
		"pushq %0\n"
		"popfq\n"
		: : "r" (pri)
		);
}

/** Return raw priority level
 *
 * Return EFLAFS.
 */
static inline pri_t cpu_priority_read(void) {
	pri_t v;
	__asm__ volatile (
		"pushfq\n"
		"popq %0\n"
		: "=r" (v)
	);
	return v;
}

extern size_t interrupt_handler_size;
extern void interrupt_handlers(void);

#endif
