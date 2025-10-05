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

ptr:	ds.l	4

SYM(SDL_MintAudio_TimerA):
	lea	(sp,-6*4),sp
	movem.l	d0-d2/a0-a2,(sp)

	pea       ptr          | Offset 2
	move.w    #141,-(sp)   | Offset 0
	trap      #14          | Call XBIOS
	addq.l    #6,sp        | Correct stack

	movem.l	(sp),d0-d2/a0-a2
	lea	(sp,+6*4),sp

	bclr	#5,0xfffffa0b:w
	move.b	#0b11011111,0xfffffa0f:w
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

whatever:
	ds.l	256*1024
whatever_end:
