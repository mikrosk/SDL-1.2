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

#ifndef _SDL_ATARI_VECTORS_H_
#define _SDL_ATARI_VECTORS_H_

#include "SDL_stdinc.h"

extern SDL_bool SDL_Atari_HookVectors(void (*install)(void), void (*restore)(void));
extern void SDL_Atari_UnhookVectors(void);

extern SDL_bool SDL_Atari_HookVbl(void (*handler)(void));
extern void SDL_Atari_UnhookVbl(void);

#endif /* _SDL_ATARI_VECTORS_H_ */
