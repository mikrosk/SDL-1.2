/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org

    This file written by Miro Kropacek (miro.kropacek@gmail.com)
*/

#include "../../video/ataricommon/symbols.h"

	/* imports */
	.globl	SYM(SDL_MintAudio_RunAudio)
	.globl	SYM(SDL_MintAudio_hasfpu)

	/* exports */
	.globl	SYM(SDL_MintAudio_TimerA)
	.globl	SYM(SDL_MintAudio_audiop)
	.globl	SYM(SDL_MintAudio_stack)

#ifndef __mcoldfire__
#define SR_OFF  0
#define PC_OFF	2
#define VEC_OFF 6
#else
#define SR_OFF  2
#define PC_OFF  4
#define VEC_OFF 0
#endif

#define CXT_LOCKED 0
#define CXT_SR     2
#define CXT_PC     4

	.text

run_audio_wrapper:
	/* Executed in Supervisor to prevent task preemption */
	move.l	sp,old_ssp

	lea	(sp,-6*4),sp
	movem.l	d0-d2/a0-a2,(sp)

	move.l	SYM(SDL_MintAudio_stack),sp

	/* Save FPU if needed */
	tst.w	SYM(SDL_MintAudio_hasfpu)
	beq.s	run_audio_wrapper_nofpu1
#ifndef __mcoldfire__
	.chip	68060
	fsave	-(sp)
	fmovem.l fpcr/fpsr/fpiar,-(sp)
	fmovem.x fp0-fp1,-(sp)
	.chip	68000
#else
	// TODO
	nop
#endif
run_audio_wrapper_nofpu1:

	/* SDL_RunAudio(audiop); */
	lea	SYM(SDL_MintAudio_RunAudio),a0
	move.l	SYM(SDL_MintAudio_audiop),-(sp)
	jsr	(a0)
	addq.l	#4,sp

	/* Restore FPU if needed */
	tst.w	SYM(SDL_MintAudio_hasfpu)
	beq.s	run_audio_wrapper_nofpu2
#ifndef __mcoldfire__
	.chip	68060
	fmovem.x (sp)+,fp0-fp1
	fmovem.l (sp)+,fpcr/fpsr/fpiar
	frestore (sp)+
	.chip	68000
#else
	// TODO
	nop
#endif
run_audio_wrapper_nofpu2:

	move.l	old_ssp,sp

	lea	(runaudio_ctx,pc),a0
#ifndef __mcoldfire__
	// TODO: m68000 test for _longframe
	clr.w	(sp,VEC_OFF+6*4)
#else
	// TODO: check for emulation
	move.w	#0x4000,(sp,VEC_OFF+6*4)
#endif
	move.l	(a0,CXT_PC),(sp,PC_OFF+6*4)
	move.w	(a0,CXT_SR),(sp,SR_OFF+6*4)

	move.w	sr,d0					| make sure that Timer B
	ori.l	#0x0600,d0				| isn't fired NOW
	move.w	d0,sr					|

	/* Clear pending bit; if the callback is too CPU heavy, we don't want
	 * to flood the system with endless pending Timer B interrupts...
	 */
	moveq	#5,d0
	bclr	d0,0xfffffa0b:w

	clr.w	(a0,CXT_LOCKED)

	movem.l	(sp),d0-d2/a0-a2
	lea	(sp,+6*4),sp
	rte

SYM(SDL_MintAudio_TimerA):
	tst.w	runaudio_ctx_locked
	bne.s	SDL_MintAudio_TimerA_done

	move.l	d0,-(sp)				| this is the +8 below
	move.l	a0,-(sp)				|

	lea	(runaudio_ctx,pc),a0

	moveq	#1,d0
	move.w	d0,(a0,CXT_LOCKED)

	// TODO: check for emulation
	move.w	(sp,SR_OFF+8),d0
	ori.l	#0x2000,d0				| Supervisor = 1
	move.w	d0,(a0,CXT_SR)

	move.l	#just_rte,d0
	move.l	d0,(a0,CXT_PC)

	move.l	(sp)+,a0
	move.l	(sp)+,d0

#ifndef __mcoldfire__
	clr.w	-(sp)
	move.l	runaudio_ctx_pc,-(sp)
	move.w	runaudio_ctx_sr,-(sp)
#else
	// TODO
	nop
#endif

SDL_MintAudio_TimerA_done:
	/* Clear in service bit ... the TOS way */
#ifndef __mcoldfire__
	move.b	#0b11011111,0xfffffa0f:w
#else
	move.b	d0,-(sp)
	move.b	#0b11011111,d0
	move.b	d0,0xfffffa0f:w
	move.b	(sp)+,d0
#endif
	/* Jump into run_audio_wrapper with interrupts on */
	rte

just_rte:
	ori.w	#0x0600,sr
	bclr	#5,0xfffffa0b:w

	clr.w	runaudio_ctx_locked
	/* Return to the caller of SDL_MintAudio_TimerA */
	rte

runaudio_ctx:
runaudio_ctx_locked:
	.dc.w	0
runaudio_ctx_sr:
	.dc.w	0
runaudio_ctx_pc:
	.dc.l	0

	.bss

SYM(SDL_MintAudio_audiop):
	.ds.l	1
SYM(SDL_MintAudio_stack):
	.ds.l	1
old_ssp:
	.ds.l	1
