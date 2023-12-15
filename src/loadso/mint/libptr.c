/*
 * imp_ptr.c - return pointer to SDL SLB
 *
 * Copyright (C) 2023 Thorsten Otto
 */
#include "slb/SDL.h"
#include <mint/osbind.h>


static long __CDECL slb_not_loaded(SLB_HANDLE slb, long fn, short nwords, ...)
{
	(void)slb;
	(void)fn;
	(void)nwords;
	(void) Cconws(SDL_SHAREDLIB_NAME " was not loaded\r\n");
	Pterm(1);
	return -32;
}


static SLB sdl_slb = { 0, slb_not_loaded };

SLB *slb_sdl_get(void)
{
	return &sdl_slb;
}
