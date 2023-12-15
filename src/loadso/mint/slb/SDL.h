/*
 * slb/SDL.h - Utility functions for the application,
 * when using the shared library.
 *
 * Copyright (C) 2023 Thorsten Otto
 */
#ifndef __SLB_SDL_H__
#define __SLB_SDL_H__ 1

#ifndef SDL_SLB
#define SDL_SLB 1
#endif

#ifndef _SDL_H
#if (defined(__MSHORT__) || defined(__PUREC__) || defined(__AHCC__)) && defined(SDL_SLB) && (defined(__atarist__) || defined(__TOS__))
typedef long sdl_int_t;
typedef unsigned long sdl_uint_t;
#define sdl_enum(x) sdl_int_t
#else
typedef int sdl_int_t;
typedef unsigned int sdl_uint_t;
#define sdl_enum(x) x
#endif
#endif

#include <SDL/SDL.h>

#ifndef __CDECL
#define __CDECL
#endif

#include <mint/slb.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SDL_SHAREDLIB_NAME "sdl.slb"

long slb_sdl_open(const char *slbpath);
void slb_sdl_close(void);
SLB *slb_sdl_get(void);

/* internal helper function */
long __CDECL sdl_slb_control(long fn, void *arg);

/* Get the full path of the shared library */
#define sdl_slb_libpath() ((const char *)sdl_slb_control(4, 0))


#ifdef __cplusplus
}
#endif

#endif /* __SLB_SDL_H__ */
