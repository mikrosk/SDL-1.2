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
#include <gem.h>


static const xbiosmode_t ttmodes[]={
	{TT_LOW,320,480,8, XBIOSMODE_C2P},
	{TT_LOW,320,240,8, XBIOSMODE_C2P|XBIOSMODE_DOUBLELINE}
};

static xbiosmode_t vdimode[]={
	{0,320,480,16, 0}
};
static void listModes(_THIS, int actually_add);
static void saveMode(_THIS, SDL_PixelFormat *vformat);
static void setMode(_THIS, xbiosmode_t *new_video_mode);
static void restoreMode(_THIS);
static void listModesVDI(_THIS, int actually_add);
static void saveModeVDI(_THIS, SDL_PixelFormat *vformat);
static void setModeVDI(_THIS, xbiosmode_t *new_video_mode);
static void restoreModeVDI(_THIS);
static int setColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);

void SDL_XBIOS_VideoInit_TT(_THIS)
{	short work_in[12], work_out[272],vdihandle,i;
	XBIOS_listModes = listModes;
	XBIOS_saveMode = saveMode;
	XBIOS_setMode = setMode;
	XBIOS_restoreMode = restoreMode;

	this->SetColors = setColors;
	
	work_in[0]=Getrez()+2;
	for(i = 1; i < 10; i++)
		work_in[i] = 1;
	work_in[10] = 2;

	v_opnvwk(work_in, &vdihandle, work_out);
	if (vdihandle != 0) {
		vdimode[0].width =  work_out[0] + 1;
		vdimode[0].height = work_out[1] + 1;
		vq_extnd(vdihandle, 1, work_out);
		vdimode[0].depth = work_out[4];
		if((vdimode[0].depth>8)&&(vdimode[0].width>=320)&&(vdimode[0].height>=480))
		{
			XBIOS_listModes = listModesVDI;
			XBIOS_saveMode = saveModeVDI;
			XBIOS_setMode = setModeVDI;
			XBIOS_restoreMode = restoreModeVDI;
		}
		v_clsvwk(vdihandle);
	}

}

static void listModes(_THIS, int actually_add)
{
	int i;

	for (i=0; i<sizeof(ttmodes)/sizeof(xbiosmode_t); i++) {
		SDL_XBIOS_AddMode(this, actually_add, &ttmodes[i]);
	}
}

static void listModesVDI(_THIS, int actually_add)
{


	SDL_XBIOS_AddMode(this, actually_add, &vdimode[0]);
}

static void saveMode(_THIS, SDL_PixelFormat *vformat)
{
	XBIOS_oldvbase=Physbase();
	XBIOS_oldvmode=EgetShift();

	switch(XBIOS_oldvmode & ES_MODE) {
		case TT_LOW:
			XBIOS_oldnumcol=256;
			break;
		case ST_LOW:
		case TT_MED:
			XBIOS_oldnumcol=16;
			break;
		case ST_MED:
			XBIOS_oldnumcol=4;
			break;
		case ST_HIGH:
		case TT_HIGH:
			XBIOS_oldnumcol=2;
			break;
	}

	if (XBIOS_oldnumcol) {
		EgetPalette(0, XBIOS_oldnumcol, XBIOS_oldpalette);
	}
}

static void saveModeVDI(_THIS, SDL_PixelFormat *vformat)
{
	
}

static void setMode(_THIS, xbiosmode_t *new_video_mode)
{
	Setscreen(-1,XBIOS_screens[0],-1);

	EsetShift(new_video_mode->number);
}

static void setModeVDI(_THIS, xbiosmode_t *new_video_mode)
{
}

static void restoreMode(_THIS)
{
	Setscreen(-1,XBIOS_oldvbase,-1);

	EsetShift(XBIOS_oldvmode);
	if (XBIOS_oldnumcol) {
		EsetPalette(0, XBIOS_oldnumcol, XBIOS_oldpalette);
	}
}

static void restoreModeVDI(_THIS)
{

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
	EsetPalette(firstcolor,ncolors,TT_palette);

	return(1);
}
