/*
 * slbids.c - callbacks to load dependant libraries
 *
 * Copyright (C) 2023 Thorsten Otto
 *
 * Functions here are part of the import library,
 * and are therefor linked to the application
 */

#include <string.h>
#include <mint/slb.h>
#include <slb/slbids.h>

SLB *__CDECL (*p_slb_get)(long lib);
long __CDECL (*p_slb_open)(long lib, const char *slbpath);
void __CDECL (*p_slb_close)(long lib);
