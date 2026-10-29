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
*/
#include "SDL_config.h"

/*
 *	Restore routines run when the process terminates before it restored
 *	what it installed
 */

#include <mint/osbind.h>

#include "SDL_atariprocterm_c.h"

#define MAX_RESTORES	16

static void (*restores[MAX_RESTORES])(void);
static void (*old_procterm)(void);

/* Called in supervisor mode when the process terminates. Each restore
   runs at most once, also when one crashes and termination comes back
   here */
static void Procterm(void)
{
	int i;

	for (i = MAX_RESTORES-1; i >= 0; i--) {
		void (*restore)(void) = restores[i];

		if (restore) {
			restores[i] = NULL;
			restore();
		}
	}

	/* TOS leaves the vector of a terminated process in place */
	Setexc(VEC_PROCTERM, old_procterm);

	old_procterm();
}

SDL_bool SDL_Atari_AddProcterm(void (*restore)(void))
{
	int i, slot = -1;

	for (i = 0; i < MAX_RESTORES; i++) {
		if (restores[i] == restore) {
			return SDL_FALSE;
		}
		if (!restores[i] && slot < 0) {
			slot = i;
		}
	}
	if (slot < 0) {
		return SDL_FALSE;
	}

	restores[slot] = restore;
	if (!old_procterm) {
		old_procterm = Setexc(VEC_PROCTERM, Procterm);
	}

	return SDL_TRUE;
}

SDL_bool SDL_Atari_RemoveProcterm(void (*restore)(void))
{
	SDL_bool found = SDL_FALSE;
	SDL_bool used = SDL_FALSE;
	int i;

	for (i = 0; i < MAX_RESTORES; i++) {
		if (restores[i] && restores[i] == restore) {
			restores[i] = NULL;
			found = SDL_TRUE;
		} else if (restores[i]) {
			used = SDL_TRUE;
		}
	}

	if (!found) {
		return SDL_FALSE;
	}

	if (!used && old_procterm) {
		Setexc(VEC_PROCTERM, old_procterm);
		old_procterm = NULL;
	}

	return SDL_TRUE;
}
