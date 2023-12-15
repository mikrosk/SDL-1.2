/*
 * libinit.c - initialization code for the shared library
 *
 * Copyright (C) 2018-2023 Thorsten Otto
 */

#include "SDL_config.h"
#include <stdio.h>
#include <mint/basepage.h>
#include <mint/mintbind.h>
#include <mint/slb.h>
#include <slb/slbids.h>
#include <mint/cookie.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <support.h>
#include "slb/SDL.h"
#include "libstrct.h"
#include "vernum.h"

long slb_init(void);
void slb_exit(void);
long slb_open(BASEPAGE *bp);
void slb_close(BASEPAGE *bp);
extern char const slb_header[];
static const BASEPAGE *my_base;


#ifdef __MSHORT__
# error "the sdl.slb must not be compiled with -mshort"
#endif

FILE *stderr;

#undef errno
/* referenced only by some math functions */
int errno;

static struct _sdl_funcs *my_funcs;


struct _sdl_funcs *sdl_get_slb_funcs(void)
{
	return my_funcs;
}


/*
 * these are not optional and cannot be set
 * to zero in the header, even if they
 * currently don't do anything
 */
long slb_init(void)
{
	const BASEPAGE *bp;
	const long *exec_longs;

	bp = (BASEPAGE *)(slb_header - 256);
	exec_longs = (const long *)((const char *)bp + 28);
	if ((exec_longs[0] == 0x283a001aL && exec_longs[1] == 0x4efb48faL) ||
		(exec_longs[0] == 0x203a001aL && exec_longs[1] == 0x4efb08faL))
		bp = (const BASEPAGE *)((const char *)bp - 228);
	my_base = bp;
	return 0;
}


void slb_exit(void)
{
}


long slb_open(BASEPAGE *bp)
{
	(void)(bp);
	/*
	 * check if SLB is already in use by this process;
	 * should not happen since MiNT should have taken care of that already
	 */
	if (my_funcs != NULL)
		return -EACCES;
	
	return 0;
}


void slb_close(BASEPAGE *bp)
{
	my_funcs = NULL;
}


/*
 * get the function table pointer passed from the application.
 * Automatically done in slb_sdl_open()
 */
__attribute__((__noinline__))
static long sdl_set_imports(struct _sdl_funcs *afuncs)
{
	if (afuncs->struct_size != sizeof(*afuncs))
		return -EINVAL;
	if (afuncs->sdl_vernum > SDL_VERSION_NUM)
		return -EBADARG;
	if (afuncs->int_size != sizeof(int))
		return -ERANGE;

	stderr = afuncs->stderr_location;
	my_funcs = afuncs;

	return 0;
}
		

__attribute__((__noinline__))
static long slb_compile_flags(void)
{
	long flags = 0;
#if defined(__mc68020__) || defined(__mc68030__) || defined(__mc68040__) || defined(__mc68060__) || defined(__mc68080__) || defined(__apollo__)
    flags |= (1L << 16);
#endif
#if defined(__mcoldfire__)
	/*
	 * use a large value here, so that we can call this function also on
	 * non-coldfire hardware; the compiler otherwise would generate a mov3q instruction
	 */
    flags |= (1L << 17);
#endif
	return flags;
}


long __CDECL sdl_slb_control(long fn, void *arg)
{
	switch ((int)fn)
	{
	case 0:
		return slb_compile_flags();
	case 1:
		return sdl_set_imports(arg);
	case 2:
		return (long)my_base;
	case 3:
		return (long)slb_header;
	case 4:
		return (long)my_base->p_cmdlin;
	}
	return -ENOSYS;
}


/*
 * extra pure-c wrappers for functions that return structures (64bit values)
 */
DECLSPEC void SDLCALL SDL_ReadLE64_purec(Uint64 *result, SDL_RWops *src);
DECLSPEC void SDLCALL SDL_ReadLE64_purec(Uint64 *result, SDL_RWops *src)
{
	*result = SDL_ReadLE64(src);
}

DECLSPEC void SDLCALL SDL_ReadBE64_purec(Uint64 *result, SDL_RWops *src);
DECLSPEC void SDLCALL SDL_ReadBE64_purec(Uint64 *result, SDL_RWops *src)
{
	*result = SDL_ReadBE64(src);
}


/*
 * C-library dependencies
 */

#undef CHECK
#if 1
#define CHECK(f, r) \
	if (sdl_get_slb_funcs()->p_##f == 0) \
	{ \
		(void) Cconws(#f " was not set\r\n"); \
		r; \
	}
#else
#define CHECK(s, r)
#endif

#define CALL(f, args) sdl_get_slb_funcs()->p_##f args


int Getcookie(long cookie, long *value)
{
	long *cookiejar = (long *) Setexc(0x5a0 / 4, (void (*)())-1);

	if (cookiejar)
	{
		while (*cookiejar)
		{
			if (*cookiejar == cookie)
			{
				if (value)
					*value = *++cookiejar;
				return C_FOUND;
			}
			cookiejar += 2;
		}
	}
	return C_NOTFOUND;
}


int atoi(const char *nptr)
{
	CHECK(atoi, return 0);
	return CALL(atoi, (nptr));
}


char *getenv(const char *name)
{
	CHECK(getenv, return 0);
	return CALL(getenv, (name));
}


int open(const char *path, int oflag, ...)
{
	va_list args;
	mode_t mode;

	CHECK(open, return -1);
	va_start(args, oflag);
	mode = va_arg(args, mode_t);
	va_end(args);
	return CALL(open, (path, oflag, mode));
}


int close(int fd)
{
	CHECK(close, return -1);
	return CALL(close, (fd));
}


ssize_t read(int fildes, void *buf, size_t nbyte)
{
	CHECK(read, return -1);
	return CALL(read, (fildes, buf, nbyte));
}


off_t lseek(int fildes, off_t offset, int whence)
{
	CHECK(lseek, return -1);
	return CALL(lseek, (fildes, offset, whence));
}


int memcmp(const void *d, const void *s, size_t n)
{
	CHECK(memcmp, return 0);
	return CALL(memcmp, (d, s, n));
}


void *memcpy(void *dest, const void *src, size_t len)
{
	CHECK(memcpy, return dest);
	return CALL(memcpy, (dest, src, len));
}
#undef memmove
void *memmove(void *dest, const void *src, size_t len) __attribute__((alias("memcpy")));


void *memset(void *dest, int c, size_t len)
{
	CHECK(memset, return dest);
	return CALL(memset, (dest, c, len));
}


int fclose(FILE *fp)
{
	CHECK(fclose, return -1);
	return CALL(fclose, (fp));
}


FILE *fopen(const char *pathname, const char *mode)
{
	CHECK(fopen, return NULL);
	return CALL(fopen, (pathname, mode));
}


int fprintf(FILE *fp, const char *format, ...)
{
	va_list args;
	int ret;
	CHECK(vfprintf, return 0);
	va_start(args, format);
	ret = CALL(vfprintf, (fp, format, args));
	va_end(args);
	return ret;
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	CHECK(fread, return 0);
	return CALL(fread, (ptr, size, nmemb, stream));
}


int fseek(FILE *stream, long offset, int whence)
{
	CHECK(fseek, return 0);
	return CALL(fseek, (stream, offset, whence));
}


long ftell(FILE *stream)
{
	CHECK(ftell, return 0);
	return CALL(ftell, (stream));
}


size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	CHECK(fwrite, return 0);
	return CALL(fwrite, (ptr, size, nmemb, stream));
}


int vsnprintf(char *str, size_t size, const char *format, va_list args)
{
	CHECK(vsnprintf, return 0);
	return CALL(vsnprintf, (str, size, format, args));
}


int sprintf(char *str, const char *format, ...)
{
	va_list args;
	int ret;
	CHECK(vsprintf, return 0);
	va_start(args, format);
	ret = CALL(vsprintf, (str, format, args));
	va_end(args);
	return ret;
}


int snprintf(char *str, size_t len, const char *format, ...)
{
	va_list args;
	int ret;
	CHECK(vsnprintf, return 0);
	va_start(args, format);
	ret = CALL(vsnprintf, (str, len, format, args));
	va_end(args);
	return ret;
}


#undef free
void free(void *ptr)
{
	CHECK(free, return);
	CALL(free, (ptr));
}

#undef malloc
void *malloc(size_t size)
{
	CHECK(malloc, return NULL);
	return CALL(malloc, (size));
}

#undef calloc
void *calloc(size_t nmemb, size_t size)
{
	return CALL(calloc, (nmemb, size));
}

void *realloc(void *ptr, size_t s)
{
	CHECK(realloc, return NULL);
	return CALL(realloc, (ptr, s));
}


int strcasecmp(const char *d, const char *s)
{
	CHECK(strcasecmp, return 0);
	return CALL(strcasecmp, (d, s));
}


char *strchr(const char *str, int c)
{
	CHECK(strchr, return NULL);
	return CALL(strchr, (str, c));
}


int strcmp(const char *d, const char *s)
{
	CHECK(strcmp, return 0);
	return CALL(strcmp, (d, s));
}


char *strcat(char *d, const char *s)
{
	CHECK(strcat, return d);
	return CALL(strcat, (d, s));
}


char *strdup(const char *s)
{
	CHECK(strdup, return 0);
	return CALL(strdup, (s));
}


char *strerror(int code)
{
	CHECK(strerror, return NULL);
	return CALL(strerror, (code));
}


size_t strlcpy(char *dst, const char *src, size_t sz)
{
	CHECK(strlcpy, return 0);
	return CALL(strlcpy, (dst, src, sz));
}


size_t strlen(const char *s)
{
	CHECK(strlen, return 0);
	return CALL(strlen, (s));
}


int strncasecmp(const char *d, const char *s, size_t n)
{
	CHECK(strncasecmp, return 0);
	return CALL(strncasecmp, (d, s, n));
}


char *strncpy(char *d, const char *s, size_t n)
{
	CHECK(strncpy, return d);
	return CALL(strncpy, (d, s, n));
}


int strncmp(const char *d, const char *s, size_t n)
{
	CHECK(strncmp, return 0);
	return CALL(strncmp, (d, s, n));
}


char *strstr(const char *str, const char *delim)
{
	CHECK(strstr, return NULL);
	return CALL(strstr, (str, delim));
}


/*
 * FIXME: signal functions are only used by SDL_fatal.
 * Maybe better to avoid using them at all in the shared lib
 */
int raise(int sig)
{
	CHECK(raise, return -1);
	return CALL(raise, (sig));
}


int sigaction(sdl_int_t sig, const struct sigaction *__restrict act, struct sigaction *__restrict oact)
{
	CHECK(sigaction, return -1);
	return CALL(sigaction, (sig, act, oact));
}


__sighandler_t signal(int signum, __sighandler_t handler)
{
	CHECK(signal, return SIG_DFL);
	return CALL(signal, (signum, handler));
}


long tfork(sdl_int_t (*func)(long), long arg)
{
	CHECK(tfork, return -1);
	return CALL(tfork, (func, arg));
}


int usleep(useconds_t usec)
{
	CHECK(usleep, return -1);
	return CALL(usleep, (usec));
}


int (toupper)(int c)
{
	CHECK(toupper, return c);
	return CALL(toupper, (c));
}

int (tolower)(int c)
{
	CHECK(tolower, return c);
	return CALL(tolower, (c));
}


static char *my_itoa(unsigned long value, char *buffer, int base)
{
	char *p;

	p = buffer + 8 * sizeof(long);
	*--p = '\0';
	do
	{
		*--p = "0123456789abcdef"[value % base];
	} while ((value /= base) != 0);

	return p;
}


void debug_print(int fn)
{
	char buf[8 * sizeof(long)];

	(void) Cconws("call #");
	(void) Cconws(my_itoa(fn, buf, 10));
	(void) Cconws("\r\n");
}
