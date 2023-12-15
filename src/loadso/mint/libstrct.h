/*
 * libstruct.h - internal header file.
 * List of functions that are exported from
 * the application to SDL
 *
 * Copyright (C) 2019-2023 Thorsten Otto
 */

#ifndef __SDLSTRUCT_H__
#define __SDLSTRUCT_H__ 1

#ifndef __CDECL
#define __CDECL
#endif

#include <mint/slb.h>

struct _sdl_funcs {
	/*
	 * sizeof(this struct), as
	 * used by the application.
	 */
	size_t struct_size;
	/*
	 * sizeof of an int of the caller.
	 * As for now, must match the one which
	 * was used to compile the library (32 bit)
	 */
	size_t int_size;
	/*
	 * version of SDL.h the caller used.
	 * As for now, should match the version that was
	 * used to compile the library.
	 */
	long sdl_vernum;
	
	FILE *stderr_location;
	SLB *__CDECL (*p_slb_get)(long lib);

	sdl_int_t __CDECL (*p_atoi)(const char *nptr);
	char *__CDECL (*p_getenv)(const char *name);

	sdl_int_t __CDECL (*p_open)(const char *, sdl_int_t, ...);
	sdl_int_t __CDECL (*p_close)(sdl_int_t);
	ssize_t __CDECL (*p_read)(sdl_int_t, void *, size_t);
	off_t __CDECL (*p_lseek)(sdl_int_t, off_t, sdl_int_t);
				
	sdl_int_t __CDECL (*p_memcmp)(const void *, const void *, size_t);
	void *__CDECL (*p_memcpy)(void *, const void *, size_t);
	void *__CDECL (*p_memset)(void *, sdl_int_t, size_t);

	sdl_int_t __CDECL (*p_fclose)(FILE *fp);
	FILE *__CDECL (*p_fopen)(const char *pathname, const char *mode);
	size_t __CDECL (*p_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
	sdl_int_t __CDECL (*p_fseek)(FILE *stream, long offset, sdl_int_t whence);
	long __CDECL (*p_ftell)(FILE *stream);
	size_t __CDECL (*p_fwrite)(const void *, size_t, size_t, FILE *);
	sdl_int_t __CDECL (*p_vfprintf)(FILE *, const char *, va_list);
	sdl_int_t (*p_vsprintf)(char *str, const char *format, va_list args);
	sdl_int_t (*p_vsnprintf)(char *str, size_t len, const char *format, va_list args);

	void *__CDECL (*p_malloc)(size_t);
	void *__CDECL (*p_calloc)(size_t, size_t);
	void *__CDECL (*p_realloc)(void *, size_t);
	void __CDECL (*p_free)(void *);

	sdl_int_t __CDECL (*p_strcasecmp)(const char *, const char *);
	char *__CDECL (*p_strcat)(char *, const char *);
	char *__CDECL (*p_strchr)(const char *, sdl_int_t);
	sdl_int_t __CDECL (*p_strcmp)(const char *, const char *);
	char *__CDECL (*p_strdup)(const char *s);
	char *__CDECL (*p_strerror)(sdl_int_t);
	size_t __CDECL (*p_strlcpy)(char *dst, const char *src, size_t sz);
	size_t __CDECL (*p_strlen)(const char *);
	sdl_int_t __CDECL (*p_strncasecmp)(const char *d, const char *s, size_t n);
	char *__CDECL (*p_strncpy)(char *, const char *, size_t);
	sdl_int_t __CDECL (*p_strncmp)(const char *, const char *, size_t);
	char *__CDECL (*p_strstr)(const char *str, const char *delim);

	sdl_int_t __CDECL (*p_raise)(sdl_int_t sig);
	sdl_int_t __CDECL (*p_sigaction)(sdl_int_t sig, const struct sigaction *__restrict act, struct sigaction *__restrict oact);
	__sighandler_t __CDECL (*p_signal)(sdl_int_t signum, __sighandler_t handler);
	long __CDECL (*p_tfork)(sdl_int_t (*func)(long), long arg);
	sdl_int_t __CDECL (*p_usleep)(useconds_t usec);
		
	sdl_int_t __CDECL (*p_toupper)(sdl_int_t c);
	sdl_int_t __CDECL (*p_tolower)(sdl_int_t c);
	
	/* room for later extensions */
	void *unused[40];
};

extern struct _sdl_funcs sdl_funcs;

long __CDECL sdl_slb_control(long fn, void *arg);
struct _sdl_funcs *sdl_get_slb_funcs(void);

#endif
