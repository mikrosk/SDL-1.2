/*
 * libclose.c - loader for library
 *
 * Copyright (C) 2023 Thorsten Otto
 */
#include "slb/SDL.h"
#include <stdio.h>
#include <mint/mintbind.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include "libstrct.h"
#include "slbload.h"




static long __CDECL slb_unloaded(SLB_HANDLE slb, long fn, short nwords, ...)
{
	(void)slb;
	(void)fn;
	(void)nwords;
	(void) Cconws(SDL_SHAREDLIB_NAME " was already unloaded\r\n");
	Pterm(1);
	return -32;
}


void slb_sdl_close(void)
{
	SLB *sdl = slb_sdl_get();

	if (!sdl)
		return;
	if (sdl->handle)
	{
		slb_unload(sdl->handle);
		sdl->handle = 0;
	}
	sdl->exec = slb_unloaded;
}
