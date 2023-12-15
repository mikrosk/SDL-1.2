/*
 * libopen.c - loader for library
 *
 * Copyright (C) 2019-2023 Thorsten Otto
 *
 * Functions here are part of the import library,
 * and are therefor linked to the application
 */
#include "slb/SDL.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <mint/cookie.h>
#include <mint/mintbind.h>
#include <slb/slbids.h>
#include <math.h>
#include <support.h>
#include "libstrct.h"
#include "slbload.h"

struct _sdl_funcs sdl_funcs;

typedef struct
{
	long key;
	long value;
} COOKIE;


static COOKIE *get_cookie_jar(void)
{
	return (COOKIE *) Setexc(0x5a0 / 4, (void (*)()) -1L);
}


static int get_cookie(long key, long *value)
{
	COOKIE *cookiejar = get_cookie_jar();

	*value = 0;
	if (cookiejar)
	{
		while (cookiejar->key)
		{
			if (cookiejar->key == key)
			{
				*value = cookiejar->value;
				return 1;
			}
			cookiejar++;
		}
	}
	return 0;
}


#if defined(__MSHORT__) || defined(__PUREC__) || defined(__AHCC__)

static void *__CDECL sdl_gcc_memset(void *s, sdl_int_t val, size_t len)
{
	return memset(s, (int)val, len);
}

static void *__CDECL sdl_gcc_memcpy(void *d, const void *s, size_t len)
{
	return memcpy(d, s, len);
}

static sdl_int_t __CDECL sdl_gcc_memcmp(const void *s, const void *d, size_t len)
{
	return memcmp(s, d, len);
}

static size_t __CDECL sdl_gcc_strlen(const char *s)
{
	return strlen(s);
}

static char *__CDECL sdl_gcc_strcpy(char *d, const char *s)
{
	return strcpy(d, s);
}

static sdl_int_t __CDECL sdl_gcc_strcmp(const char *d, const char *s)
{
	return strcmp(d, s);
}

static void *__CDECL sdl_gcc_malloc(size_t s)
{
	return malloc(s);
}

static void __CDECL sdl_gcc_free(void *s)
{
	free(s);
}


#define S(x) sdl_funcs.p_##x = sdl_gcc_##x

#else

#define S(x) sdl_funcs.p_##x = x

#endif


static SLB *__CDECL callback_slb_get(long lib)
{
	switch ((int) lib)
	{
	case LIB_SDL:
		return slb_sdl_get();
	}
	return NULL;
}


long slb_sdl_open(const char *slbpath)
{
	long ret;
	long flags;
	long cpu;
	SLB *sdl;
	
	sdl_funcs.struct_size = sizeof(sdl_funcs);
	sdl_funcs.int_size = sizeof(sdl_int_t);
	sdl_funcs.sdl_vernum = SDL_COMPILEDVERSION;

	if ((sdl_funcs.p_slb_get = p_slb_get) == NULL)
		sdl_funcs.p_slb_get = callback_slb_get;
	sdl_funcs.stderr_location = stderr;

	S(atoi);
	S(getenv);

	S(open);
	S(close);
	S(read);
	S(lseek);
	
	S(memcmp);
	S(memcpy);
	S(memset);

	S(fclose);
	S(fopen);
	S(fread);
	S(fseek);
	S(ftell);
	S(fwrite);
	S(vfprintf);
	S(vsprintf);
	S(vsnprintf);

	S(malloc);
	S(calloc);
	S(realloc);
	S(free);

	S(strcasecmp);
	S(strcat);
	S(strchr);
	S(strcmp);
	S(strdup);
	S(strerror);
	S(strlcpy);
	S(strlen);
	S(strncasecmp);
	S(strncpy);
	S(strncmp);
	S(strstr);

	S(raise);
	S(sigaction);
	S(signal);
	S(tfork);
	S(usleep);

	S(toupper);
	S(tolower);

#undef S

	sdl = sdl_funcs.p_slb_get(LIB_SDL);
	if (!sdl)
		return -EBADF;
	if (sdl->handle)
		return 0;

	ret = slb_load(SDL_SHAREDLIB_NAME, slbpath, SDL_COMPILEDVERSION, &sdl->handle, &sdl->exec);
	if (ret < 0)
	{
		slb_sdl_close();
		return ret;
	}

	/*
	 * check compile flags; that function should be as simple as to just return a constant
	 * and we can hopefully call it even on mismatched configurations
	 */
	flags = sdl_slb_control(0, 0);
	get_cookie(C__CPU, &cpu);
	if (cpu >= 20)
	{
		/* should be able to use a 000 library, anyways */
	} else
	{
		if (flags & (1L << 16))
		{
			/* cpu is not 020+, but library was compiled for it */
			slb_sdl_close();
			return -EINVAL;
		}
	}
#if defined(__mcoldfire__)
	/* if cpu is cf, but library was not compiled for it... */
	if (!(flags & (1L << 17)))
#else
	/* if cpu is not cf, but library was compiled for it... */
	if (flags & (1L << 17))
#endif
	{
		slb_sdl_close();
		return -EINVAL;
	}
	
	ret = sdl_slb_control(1, &sdl_funcs);
	if (ret < 0)
	{
		slb_sdl_close();
		return ret;
	}
	
	return ret;
}
