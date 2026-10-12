/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/*
	TT Xbios video functions

	Patrice Mandin
*/

#include <mint/cookie.h>
#include <mint/osbind.h>

#include "../SDL_sysvideo.h"

#include "SDL_xbios.h"
#include "SDL_xbios_nova.h"

static const xbiosmode_t ttmodes[]={
	{TT_LOW>>8,320,480,8,XBIOSMODE_C2P},
	{TT_LOW>>8,320,240,8,XBIOSMODE_C2P|XBIOSMODE_DOUBLELINE}
};

/* Triple buffering: palette to set in the next VBL */
static volatile struct {
	short pending;
	short first;
	short last;
	Uint16 colors[256];
} vbl_palette;

static void listModes(_THIS, int actually_add);
static void setMode(_THIS, const xbiosmode_t *new_video_mode);
static int setColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static SDL_bool initTripleBuffer(_THIS);
static void vblTripleBuffer(void);

void SDL_XBIOS_VideoInit_TT(_THIS)
{
	XBIOS_listModes = listModes;
	XBIOS_setMode = setMode;
	XBIOS_initTripleBuffer = initTripleBuffer;

	this->SetColors = setColors;
}

static void listModes(_THIS, int actually_add)
{
	int i;

	for (i=0; i<sizeof(ttmodes)/sizeof(xbiosmode_t); i++) {
		SDL_XBIOS_AddMode(this, actually_add, &ttmodes[i]);
	}
}

static void setMode(_THIS, const xbiosmode_t *new_video_mode)
{
	Setscreen(-1,XBIOS_screens[0],new_video_mode->number);
}

static int setColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	int	i, r,g,b;

	for(i = 0; i < ncolors; i++) {
		r = colors[i].r;
		g = colors[i].g;
		b = colors[i].b;

		TT_palette[i]=((r>>4)<<8)|((g>>4)<<4)|(b>>4);
	}

	if (XBIOS_flipTripleBuffer) {
		/* EsetPalette() sets the palette immediately, the frame flipped
		   before is displayed only from the next VBL on */
		const int lastcolor = firstcolor + ncolors - 1;
		Uint16 sr;
		void *ssp = SDL_XBIOS_LockVbl(&sr);

		if (!vbl_palette.pending || firstcolor < vbl_palette.first) {
			vbl_palette.first = firstcolor;
		}
		if (!vbl_palette.pending || lastcolor > vbl_palette.last) {
			vbl_palette.last = lastcolor;
		}
		for (i = 0; i < ncolors; i++) {
			vbl_palette.colors[firstcolor + i] = TT_palette[i];
		}
		vbl_palette.pending = 1;

		SDL_XBIOS_UnlockVbl(ssp, sr);
	} else {
		EsetPalette(firstcolor,ncolors,TT_palette);
	}

	return(1);
}

/* Triple buffering as on ST/STE, the VBL handler only sets the palette */
static SDL_bool initTripleBuffer(_THIS)
{
	vbl_palette.pending = 0;

	if (!SDL_XBIOS_InstallVbl(vblTripleBuffer)) {
		return SDL_FALSE;
	}

	return SDL_XBIOS_InitTripleBuffer(this);
}

static void vblTripleBuffer(void)
{
	if (vbl_palette.pending) {
		volatile Uint16 *tt_palette = (volatile Uint16 *) 0xFFFF8400;
		int i;

		for (i = vbl_palette.first; i <= vbl_palette.last; i++) {
			tt_palette[i] = vbl_palette.colors[i];
		}
		vbl_palette.pending = 0;
	}
}
