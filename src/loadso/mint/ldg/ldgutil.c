/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Some usefull functions (for LDG kernel & client application)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Patch Coldfire Vincent Riviere
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "ldg.h"
#include "global.h"

/*#include <gem.h>*/
#if defined(__PUREC__) || defined(__VBCC__)
#include <tos.h>
#endif
#if defined(__GNUC__) || defined(__SOZOBONX__)
#include <osbind.h>
#define BASPAG	BASEPAGE
#endif

#define MagX_COOKIE     0x4D616758L		/* 'MagX' */
#define MiNT_COOKIE     0x4D694E54L		/* 'MiNT' */


int ldg_cookie(long cookie, long *value)
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
				return 1;
			}
			cookiejar += 2;
		}
	}
	return 0;
}



/*
 * Error handling: the error code is
 * written to the field of the LDGM cookie dedicated to
 * this purpose.
 */
void ldg_set_error(int code)
{
	LDG_INFOS *cook;

	if (ldg_cookie(LDG_COOKIE, (long *) &cook) && cook->version >= 0x0210)
		cook->error = (short) (code & 0xFFFF);
}


#if 0
/*
 * Debug function
 * New version if the <format> string is
 * the format of a GEM alert, use form_alert
 */
void ldg_debug(const char *format, ...)
{
	char *path = NULL;
	char debug[255];
	va_list arglist;
	FILE *log;

	/* Temporary, one could also use the cookie
	 * because it would be more flexible
	 */
	shel_envrn(&path, "LDG_DEBUG=");
	if (path == NULL)
		return;

	/* format */
	if (format)
	{
		va_start(arglist, format);
		vsprintf(debug, format, arglist);
		va_end(arglist);
		if (*format == '[')
			form_alert(1, debug);
		else
		{
			log = fopen(path, "a");
			if (log)
				fputs(debug, log);
		}
	} else
	{
		log = fopen(path, "w");
		if (log)
			fclose(log);
	}
}
#endif


#ifndef __mcoldfire__
/*
 *  ldg_cpush
 *
 * Empty caches and data processor instruction
 * By calling the asm function "cpush".
 * This code can be called from the 68040 and must be
 * executed in supervisor mode (hence the use of Supexec).
 *
 * 1st version April 17, 2002 by Arnaud Bercegeay <bercegeay@atari.org>
 */

#define CPU_COOKIE 0x5F435055UL			/* _CPU */

void ldg_cpush(void)
{
	static long _cpu = -1;

	static unsigned short const do_cpush[] = {
		0x4E71,							/* NOP         ; 1 nop useful for some 68040 */
		0xF4F8,							/* CPUSHA BC   ; emptying caches data & instruction */
		0x4E75							/* RTS         ; end of the procedure */
	};

	if (_cpu < 0)
	{
		ldg_cookie(CPU_COOKIE, &_cpu);
		_cpu &= 0xFFFF;
	}

	if (_cpu > 30)
		Supexec((long (*)()) do_cpush);
}
#endif
