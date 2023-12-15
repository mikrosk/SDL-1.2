/*
 * lib_imp.h - internal header for building the import library
 *
 * Copyright (C) 2023 Thorsten Otto
 */
#include "SDL_config.h"
#include <mint/mintbind.h>
#include <mint/slb.h>
#include <errno.h>
#include "symbols.h"

SLB *slb_sdl_get(void);

#undef SLB_NWORDS
#define SLB_NWORDS(_nargs) ((((long)(_nargs) * 2 + 1l) << 16) | (long)(_nargs))
#undef SLB_NARGS
#define SLB_NARGS(_nargs) SLB_NWORDS(_nargs)

#ifdef __GNUC__

#define PUSH_FNQ(fn) \
		" moveq %[fn],%%d1\n" /* push function number */ \
		" move.l %%d1,-(%%a7)\n"

#define PUSH_FNL(fn) \
		" move.l %[fn],-(%%a7)\n" /* push function number */

#define LIBFUNC_(_fn, name, _nargs, pushfn) \
static void __attribute__((used)) __ ## name ## _entry(void) \
{ \
	register SLB *sdl __asm__("a0"); \
 \
	__asm__ __volatile__ ( \
		" .globl " C_SYMBOL_NAME(name) "\n" \
C_SYMBOL_NAME(name) ":\n"); \
 \
	sdl = slb_sdl_get(); \
 \
	__asm__ __volatile__ ( \
		" move.l (%%a7)+,%%d0\n" /* get return pc */ \
		" move.l %[nargs],-(%%a7)\n"  /* push number of args */ \
		pushfn /* push function number */ \
		" move.l (%[sdl]),-(%%a7)\n" /* push SLB handle */ \
		" move.l %%d0,-(%%a7)\n" /* push return pc */ \
		" move.l 4(%[sdl]),%[sdl]\n" /* get exec function */ \
		" jmp (%[sdl])\n"          /* go for it */ \
	: \
	: [sdl]"r"(sdl), [fn]"i"(_fn), [nargs]"i"(SLB_NARGS(_nargs)) \
	: "cc", "memory" \
	); \
	__builtin_unreachable(); \
}

#define LIBFUNC(_fn, name, _nargs) LIBFUNC_(_fn, name, _nargs, PUSH_FNQ(fn))
#define LIBFUNC2(_fn, name, _nargs) LIBFUNC_(_fn, name, _nargs, PUSH_FNL(fn))
#define LIBFUNCRET64(_fn, orig_fn, name, _nargs) LIBFUNC_(orig_fn, name, _nargs, PUSH_FNQ(fn))
#define LIBFUNC2RET64(_fn, orig_fn, name, _nargs) LIBFUNC_(orig_fn, name, _nargs, PUSH_FNL(fn))

#define SDL_ReadLE64_gnuc SDL_ReadLE64
#define SDL_ReadBE64_gnuc SDL_ReadBE64

#endif /* __GNUC__ */

#if defined(__AHCC__)

#elif defined(__PUREC__)

static long move_l_a7_a1(void *, long) 0x225f; /* move.l (a7)+,a1 get return pc */
static long push_d1(long, long) 0x2f01;        /* move.l d1,-(a7) pushes #args */
static long push_d0(long) 0x2f00;              /* move.l d0,-(a7) pushes function # */
static void *push_a0(long) 0x2f10;             /* move.l (a0),-(a7) pushes SLB_HANDLE */
static void *move_4_a0_1(void *) 0x0004;       /* move.l 4(a0),a0 get exec function */
static void *move_4_a0_2(void *) 0x2068;
static void *push_a1(void *) 0x2f09;           /* move.l a1,-(a7) pushes return pc */
static void *jmp_a0(void *) 0x4ed0;            /* jmp (a0) */

#define LIBFUNC(_fn, name, _nargs) \
void name(void) \
{ \
	jmp_a0(push_a1(move_4_a0_1(move_4_a0_2(push_a0(push_d0(push_d1(move_l_a7_a1(slb_sdl_get(), _fn), SLB_NARGS(_nargs)))))))); \
}
#define LIBFUNC2(_fn, name, _nargs) LIBFUNC(_fn, name, _nargs)
#define LIBFUNCRET64(_fn, orig_fn, name, _nargs) LIBFUNC(_fn, name, _nargs)
#define LIBFUNC2RET64(_fn, orig_fn, name, _nargs) LIBFUNCRET64(_fn, name, _nargs)

#endif

#define NOFUNC
