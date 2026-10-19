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
 *	Interrupt vectors and VBL queue handler, removed again when the
 *	process terminates
 */

#include <mint/basepage.h>
#include <mint/cookie.h>
#include <mint/mintbind.h>
#include <mint/osbind.h>
#include <mint/sysvars.h>

#include "SDL_atarivectors_c.h"

static void (*restore_vectors)(void);
static void (*vbl_handler)(void);
static void (*old_procterm)(void);

/* The vectors and the VBL handler run whichever process is active */
static SDL_bool CheckAccess(void)
{
	Uint32 flags;

	if (Getcookie(C_MiNT, NULL) != C_FOUND)
		return SDL_TRUE;

	if (Mvalidate(0, _base->p_tbase, _base->p_tlen + _base->p_dlen + _base->p_blen, &flags) < 0)
		return SDL_FALSE;

	if (((flags+0x10)&0xf0) != MX_SUPERVISOR && ((flags+0x10)&0xf0) != MX_GLOBAL)
		return SDL_FALSE;

	return SDL_TRUE;
}

/* Supervisor only */
static long AddVbl(void)
{
	void (**queue)(void) = (void (**)(void)) *_vblqueue;
	int i;

	for (i = 0; i < *nvbls; i++) {
		if (queue[i] == NULL) {
			queue[i] = vbl_handler;
			return 1;
		}
	}
	return 0;
}

/* Supervisor only */
static void RemoveVbl(void)
{
	void (**queue)(void) = (void (**)(void)) *_vblqueue;
	int i;

	for (i = 0; i < *nvbls; i++) {
		if (queue[i] == vbl_handler) {
			queue[i] = NULL;
			break;
		}
	}
}

/* Called in supervisor mode when the process terminates */
static void Procterm(void)
{
	if (vbl_handler) {
		RemoveVbl();
	}

	if (restore_vectors) {
		restore_vectors();
	}

	/* TOS leaves the vector of a terminated process in place */
	Setexc(VEC_PROCTERM, old_procterm);

	old_procterm();
}

SDL_bool SDL_Atari_HookVectors(void (*install)(void), void (*restore)(void))
{
	if (!CheckAccess()) {
		return SDL_FALSE;
	}

	Supexec(install);

	restore_vectors = restore;
	old_procterm = Setexc(VEC_PROCTERM, Procterm);

	return SDL_TRUE;
}

void SDL_Atari_UnhookVectors(void)
{
	if (restore_vectors) {
		Supexec(restore_vectors);
		restore_vectors = NULL;

		Setexc(VEC_PROCTERM, old_procterm);
	}
}

SDL_bool SDL_Atari_HookVbl(void (*handler)(void))
{
	if (!CheckAccess()) {
		return SDL_FALSE;
	}

	vbl_handler = handler;
	if (!Supexec(AddVbl)) {
		vbl_handler = NULL;
		return SDL_FALSE;
	}

	return SDL_TRUE;
}

void SDL_Atari_UnhookVbl(void)
{
	if (vbl_handler) {
		Supexec(RemoveVbl);
		vbl_handler = NULL;
	}
}
