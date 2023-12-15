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

/** @file SDL_exports.h
 *  internal header with definitions of all exported functions
 *
 *  Copyright (C) 2023 Thorsten Otto
 */

#ifndef LIBFUNC
#error "LIBFUNC must be defined before including this file"
#endif
#ifndef LIBFUNC2
#define LIBFUNC2(_fn, name, _nargs) LIBFUNC(_fn, name, _nargs)
#endif
#ifndef LIBFUNCRET64
#define LIBFUNCRET64(_fn, orig_fn, name, _nargs) LIBFUNC(_fn, name, _nargs)
#endif
#ifndef LIBFUNC2RET64
#define LIBFUNC2RET64(_fn, orig_fn, name, _nargs) LIBFUNC2(_fn, name, _nargs)
#endif

/* Atari specific functions */
	LIBFUNC(sdl_slb_control, 2)

/* Version */
	LIBFUNC(SDL_Linked_Version, 0)

/* Basic features */
	LIBFUNC(SDL_Init, 1)
	LIBFUNC(SDL_InitSubSystem, 1)
	LIBFUNC(SDL_QuitSubSystem, 1)
	LIBFUNC(SDL_WasInit, 1)
	LIBFUNC(SDL_Quit, 0)
	LIBFUNC(SDL_GetAppState, 0)

/* Audio */
	LIBFUNC(SDL_AudioInit, 1)
	LIBFUNC(SDL_AudioQuit, 0)
	LIBFUNC(SDL_AudioDriverName, 1)
	LIBFUNC(SDL_OpenAudio, 2)
	LIBFUNC(SDL_GetAudioStatus, 0)
	LIBFUNC(SDL_PauseAudio, 1)
	LIBFUNC(SDL_LoadWAV_RW, 5)
	LIBFUNC(SDL_FreeWAV, 1)
	LIBFUNC(SDL_BuildAudioCVT, 1)
	LIBFUNC(SDL_ConvertAudio, 1)
	LIBFUNC(SDL_MixAudio, 4)
	LIBFUNC(SDL_LockAudio, 0)
	LIBFUNC(SDL_UnlockAudio, 0)
	LIBFUNC(SDL_CloseAudio, 0)
	LIBFUNC(SDL_Audio_SetCaption, 1)

/* CDROM */
	LIBFUNC(SDL_CDNumDrives, 0)
	LIBFUNC(SDL_CDName, 1)
	LIBFUNC(SDL_CDOpen, 1)
	LIBFUNC(SDL_CDStatus, 1)
	LIBFUNC(SDL_CDPlayTracks, 5)
	LIBFUNC(SDL_CDPlay, 3)
	LIBFUNC(SDL_CDPause, 1)
	LIBFUNC(SDL_CDResume, 1)
	LIBFUNC(SDL_CDStop, 1)
	LIBFUNC(SDL_CDEject, 1)
	LIBFUNC(SDL_CDClose, 1)

/* cpuinfo (do we really need this?) */
	LIBFUNC(SDL_HasRDTSC, 0)
	LIBFUNC(SDL_HasMMX, 0)
	LIBFUNC(SDL_HasMMXExt, 0)
	LIBFUNC(SDL_Has3DNow, 0)
	LIBFUNC(SDL_Has3DNowExt, 0)
	LIBFUNC(SDL_HasSSE, 0)
	LIBFUNC(SDL_HasSSE2, 0)
	LIBFUNC(SDL_HasAltiVec, 0)

/* Error functions */
	LIBFUNC(SDL_SetError, 100)
	LIBFUNC(SDL_GetError, 0)
	LIBFUNC(SDL_ClearError, 0)
	LIBFUNC(SDL_Error, 1)
	LIBFUNC(SDL_GetErrorMsg, 2)

/* Events */
	LIBFUNC(SDL_PumpEvents, 0)
	LIBFUNC(SDL_PeepEvents, 4)
	LIBFUNC(SDL_PollEvent, 1)
	LIBFUNC(SDL_WaitEvent, 1)
	LIBFUNC(SDL_PushEvent, 1)
	LIBFUNC(SDL_SetEventFilter, 1)
	LIBFUNC(SDL_EventState, 2)

/* Joystick */
	LIBFUNC(SDL_NumJoysticks, 0)
	LIBFUNC(SDL_JoystickName, 1)
	LIBFUNC(SDL_JoystickOpen, 1)
	LIBFUNC(SDL_JoystickOpened, 1)
	LIBFUNC(SDL_JoystickIndex, 1)
	LIBFUNC(SDL_JoystickNumAxes, 1)
	LIBFUNC(SDL_JoystickNumBalls, 1)
	LIBFUNC(SDL_JoystickNumHats, 1)
	LIBFUNC(SDL_JoystickNumButtons, 1)
	LIBFUNC(SDL_JoystickUpdate, 1)
	LIBFUNC(SDL_JoystickEventState, 1)
	LIBFUNC(SDL_JoystickGetAxis, 2)
	LIBFUNC(SDL_JoystickGetHat, 2)
	LIBFUNC(SDL_JoystickGetBall, 4)
	LIBFUNC(SDL_JoystickGetButton, 2)
	LIBFUNC(SDL_JoystickClose, 1)

/* Keyboard */
	LIBFUNC(SDL_EnableUNICODE, 1)
	LIBFUNC(SDL_EnableKeyRepeat, 1)
	LIBFUNC(SDL_GetKeyRepeat, 1)
	LIBFUNC(SDL_GetKeyState, 1)
	LIBFUNC(SDL_GetModState, 0)
	LIBFUNC(SDL_SetModState, 1)
	LIBFUNC(SDL_GetKeyName, 1)

/* Dynamic loading through LDG */
	LIBFUNC(SDL_LoadObject, 1)
	LIBFUNC(SDL_LoadFunction, 1)
	LIBFUNC(SDL_UnloadObject, 1)

/* SDLmain: nothing */

/* Mouse */
	LIBFUNC(SDL_GetMouseState, 2)
	LIBFUNC(SDL_GetRelativeMouseState, 2)
	LIBFUNC(SDL_WarpMouse, 2)
	LIBFUNC(SDL_CreateCursor, 6)
	LIBFUNC(SDL_SetCursor, 1)
	LIBFUNC(SDL_GetCursor, 0)
	LIBFUNC(SDL_FreeCursor, 1)
	LIBFUNC(SDL_ShowCursor, 1)

/* Mutex */
	LIBFUNC(SDL_CreateMutex, 0)
	LIBFUNC(SDL_mutexP, 1)
	LIBFUNC(SDL_mutexV, 1)
	LIBFUNC(SDL_DestroyMutex, 1)
	LIBFUNC(SDL_CreateSemaphore, 1)
	LIBFUNC(SDL_DestroySemaphore, 1)
	LIBFUNC(SDL_SemWait, 1)
	LIBFUNC(SDL_SemTryWait, 1)
	LIBFUNC(SDL_SemWaitTimeout, 2)
	LIBFUNC(SDL_SemPost, 1)
	LIBFUNC(SDL_SemValue, 1)
	LIBFUNC(SDL_CreateCond, 0)
	LIBFUNC(SDL_DestroyCond, 1)
	LIBFUNC(SDL_CondSignal, 1)
	LIBFUNC(SDL_CondBroadcast, 1)
	LIBFUNC(SDL_CondWait, 2)
	LIBFUNC(SDL_CondWaitTimeout, 3)

/* RWops */
	LIBFUNC(SDL_RWFromFile, 2)
	LIBFUNC(SDL_RWFromFP, 2)
	LIBFUNC(SDL_RWFromMem, 2)
	LIBFUNC(SDL_RWFromConstMem, 2)
	LIBFUNC(SDL_AllocRW, 0)
	LIBFUNC(SDL_FreeRW, 1)
	LIBFUNC(SDL_ReadLE16, 1)
	LIBFUNC(SDL_ReadBE16, 1)
	LIBFUNC(SDL_ReadLE32, 1)
	LIBFUNC(SDL_ReadBE32, 1)
	LIBFUNC(SDL_ReadLE64_gnuc, 1)
	LIBFUNC(SDL_ReadBE64_gnuc, 1)
	LIBFUNC(SDL_WriteLE16, 2)
	LIBFUNC(SDL_WriteBE16, 2)
	LIBFUNC(SDL_WriteLE32, 2)
	LIBFUNC(SDL_WriteBE32, 2)
	LIBFUNC(SDL_WriteLE64, 3)
	LIBFUNC(SDL_WriteBE64, 3)

/* Standard library. Most of these are directed already to the C-library */
	LIBFUNC(SDL_revcpy, 3)
	LIBFUNC(SDL_strrev, 1)
	LIBFUNC(SDL_strupr, 1)
	LIBFUNC(SDL_strlwr, 1)
	LIBFUNC(SDL_lltoa, 3)
	LIBFUNC(SDL_ulltoa, 3)
	LIBFUNC(SDL_iconv_open, 2)
	LIBFUNC(SDL_iconv_close, 2)
	LIBFUNC(SDL_iconv, 5)
	LIBFUNC(SDL_iconv_string, 4)

/* Window manager */
	LIBFUNC(SDL_GetWMInfo, 1)
	LIBFUNC(SDL_WM_SetCaption, 2)
	LIBFUNC(SDL_WM_GetCaption, 2)
	LIBFUNC(SDL_WM_SetIcon, 2)
	LIBFUNC(SDL_WM_IconifyWindow, 0)
	LIBFUNC(SDL_WM_ToggleFullScreen, 1)
	LIBFUNC(SDL_WM_GrabInput, 1)
	
/* Threads */
	LIBFUNC(SDL_CreateThread, 2)
	LIBFUNC(SDL_ThreadID, 0)
	LIBFUNC(SDL_GetThreadID, 1)
	LIBFUNC(SDL_WaitThread, 2)
	LIBFUNC(SDL_KillThread, 1)

/* Timer */
	LIBFUNC(SDL_GetTicks, 0)
	LIBFUNC(SDL_Delay, 1)
	LIBFUNC(SDL_SetTimer, 2)
	LIBFUNC(SDL_AddTimer, 3)
	LIBFUNC(SDL_RemoveTimer, 1)

/* Video */
	LIBFUNC(SDL_VideoInit, 2)
	LIBFUNC(SDL_VideoQuit, 0)
	LIBFUNC(SDL_VideoDriverName, 2)
	LIBFUNC(SDL_GetVideoSurface, 0)
	LIBFUNC(SDL_GetVideoInfo, 0)
	LIBFUNC(SDL_VideoModeOK, 4)
	LIBFUNC(SDL_ListModes, 2)
	LIBFUNC(SDL_SetVideoMode, 4)
	LIBFUNC(SDL_UpdateRects, 3)
	LIBFUNC(SDL_UpdateRect, 5)
	LIBFUNC(SDL_Flip, 1)
	LIBFUNC(SDL_SetGamma, 3)
	LIBFUNC(SDL_GetGamma, 3)
	LIBFUNC(SDL_SetGammaRamp, 3)
	LIBFUNC(SDL_GetGammaRamp, 3)
	LIBFUNC(SDL_SetColors, 4)
	LIBFUNC(SDL_SetPalette, 5)
	LIBFUNC(SDL_MapRGB, 5)
	LIBFUNC(SDL_MapRGBA, 6)
	LIBFUNC(SDL_GetRGB, 5)
	LIBFUNC(SDL_GetRGBA, 6)
	LIBFUNC(SDL_CreateRGBSurface, 8)
	LIBFUNC(SDL_CreateRGBSurfaceFrom, 9)
	LIBFUNC(SDL_FreeSurface, 1)
	LIBFUNC(SDL_LockSurface, 1)
	LIBFUNC(SDL_UnlockSurface, 1)
	LIBFUNC(SDL_LoadBMP_RW, 2)
	LIBFUNC(SDL_SaveBMP_RW, 3)
	LIBFUNC(SDL_SetColorKey, 3)
	LIBFUNC(SDL_SetAlpha, 3)
	LIBFUNC(SDL_SetClipRect, 2)
	LIBFUNC(SDL_GetClipRect, 2)
	LIBFUNC(SDL_ConvertSurface, 3)
	LIBFUNC(SDL_UpperBlit, 4)
	LIBFUNC(SDL_LowerBlit, 4)
	LIBFUNC(SDL_FillRect, 3)
	LIBFUNC(SDL_DisplayFormat, 1)
	LIBFUNC(SDL_DisplayFormatAlpha, 1)
	LIBFUNC(SDL_CreateYUVOverlay, 4)
	LIBFUNC(SDL_LockYUVOverlay, 1)
	LIBFUNC(SDL_UnlockYUVOverlay, 1)
	LIBFUNC(SDL_DisplayYUVOverlay, 1)
	LIBFUNC(SDL_FreeYUVOverlay, 1)
	LIBFUNC(SDL_SoftStretch, 4)

/* OpenGL */
	LIBFUNC(SDL_GL_LoadLibrary, 1)
	LIBFUNC(SDL_GL_GetProcAddress, 1)
	LIBFUNC(SDL_GL_SetAttribute, 2)
	LIBFUNC(SDL_GL_GetAttribute, 2)
	LIBFUNC(SDL_GL_SwapBuffers, 0)
	LIBFUNC(SDL_GL_UpdateRects, 2)
	LIBFUNC(SDL_GL_Lock, 0)
	LIBFUNC(SDL_GL_Unlock, 0)

/* extra pure-c wrappers */
	LIBFUNCRET64(115, SDL_ReadLE64_purec, 2)
	LIBFUNCRET64(116, SDL_ReadBE64_purec, 2)

#undef LIBFUNC
#undef LIBFUNC2
#undef LIBFUNCRET64
#undef LIBFUNC2RET64
#undef NOFUNC
