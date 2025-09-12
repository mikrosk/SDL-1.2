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
#include "SDL_config.h"

#include <usound.h>

#include "SDL_rwops.h"
#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"
#include "SDL_mintaudio.h"

/* The tag name used by DUMMY audio */
#define MINTAUD_DRIVER_NAME         "mint"

/* Audio driver functions */
static int MINTAUD_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void MINTAUD_WaitAudio(_THIS);
static void MINTAUD_PlayAudio(_THIS);
static Uint8 *MINTAUD_GetAudioBuf(_THIS);
static void MINTAUD_CloseAudio(_THIS);

/* Audio driver bootstrap functions */
static int MINTAUD_Available(void)
{
	const char *envr = SDL_getenv("SDL_AUDIODRIVER");
	if (envr && (SDL_strcmp(envr, MINTAUD_DRIVER_NAME) == 0)) {
		return(1);
	}
	return(0);
}

static void MINTAUD_DeleteDevice(SDL_AudioDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_AudioDevice *MINTAUD_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( this ) {
		SDL_memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *this->hidden));
	}
	if ( (this == NULL) || (this->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( this ) {
			SDL_free(this);
		}
		return(0);
	}
	SDL_memset(this->hidden, 0, (sizeof *this->hidden));

	/* Set the function pointers */
	this->OpenAudio = MINTAUD_OpenAudio;
	this->WaitAudio = MINTAUD_WaitAudio;
	this->PlayAudio = MINTAUD_PlayAudio;
	this->GetAudioBuf = MINTAUD_GetAudioBuf;
	this->CloseAudio = MINTAUD_CloseAudio;

	this->free = MINTAUD_DeleteDevice;

	return this;
}

AudioBootStrap MINTAUD_bootstrap = {
	MINTAUD_DRIVER_NAME, "SDL mint audio driver",
	MINTAUD_Available, MINTAUD_CreateDevice
};

/* This function waits until it is possible to write a full sound buffer */
static void MINTAUD_WaitAudio(_THIS)
{
	/* Don't block on first calls to simulate initial fragment filling. */
	if (this->hidden->initial_calls)
		this->hidden->initial_calls--;
	else
		SDL_Delay(this->hidden->write_delay);
}

static void MINTAUD_PlayAudio(_THIS)
{
	/* no-op...this is a null driver. */
}

static Uint8 *MINTAUD_GetAudioBuf(_THIS)
{
	return(this->hidden->mixbuf);
}

static void MINTAUD_CloseAudio(_THIS)
{
	if ( this->hidden->mixbuf != NULL ) {
		SDL_FreeAudioMem(this->hidden->mixbuf);
		this->hidden->mixbuf = NULL;
	}
}

static int MINTAUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	float bytes_per_sec = 0.0f;

	/* Allocate mixing buffer */
	this->hidden->mixlen = spec->size;
	this->hidden->mixbuf = (Uint8 *) SDL_AllocAudioMem(this->hidden->mixlen);
	if ( this->hidden->mixbuf == NULL ) {
		return(-1);
	}
	SDL_memset(this->hidden->mixbuf, spec->silence, spec->size);

	bytes_per_sec = (float) (((spec->format & 0xFF) / 8) *
	                   spec->channels * spec->freq);

	/*
	 * We try to make this request more audio at the correct rate for
	 *  a given audio spec, so timing stays fairly faithful.
	 * Also, we have it not block at all for the first two calls, so
	 *  it seems like we're filling two audio fragments right out of the
	 *  gate, like other SDL drivers tend to do.
	 */
	this->hidden->initial_calls = 2;
	this->hidden->write_delay =
	               (Uint32) ((((float) spec->size) / bytes_per_sec) * 1000.0f);

	/* We're ready to rock and roll. :-) */
	return(0);
}

