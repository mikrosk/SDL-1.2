# atari/Makefile
#
# Copyright (c) 2019-2023 Thorsten Otto

#
# to configure and build the test programs, run
# PKG_CONFIG=pkg-config PKG_CONFIG_PATH=.. SDL_LIBS="-L../src/loadso/mint -lSDL.slb -lm" SDL_CFLAGS="-DSDL_SLB -I../src/loadso/mint -I../include" ./configure --prefix=/usr --host=m68k-atari-mint
#
# Then run them with
# SLBPATH=../src/loadso/mint ./checkkeys
# etc.
#

CROSS_PREFIX=m68k-atari-mint-

CC = $(CROSS_PREFIX)gcc

AR = $(CROSS_PREFIX)ar
RANLIB = $(CROSS_PREFIX)ranlib
STRIP = $(CROSS_PREFIX)strip
AWK = awk

srcdir = .
top_srcdir = ../../..

#CPU_CFLAGS=-m68020-60
WARN = -Wall -Wstrict-prototypes
#WARN += -Wmissing-prototypes
#WARN += -Werror
CFLAGS = $(CPU_CFLAGS) -O2 -fomit-frame-pointer $(WARN)
CPPFLAGS = -I. -I$(srcdir) -I$(top_srcdir)/include
SFLAGS = $(CFLAGS) -DSDL_SLB
# slbexec.S must also be compilabe by pure-c, so we cannot preprocess it
# figure out the symbol prefix here
USER_LABEL_PREFIX := $(shell echo '__USER_LABEL_PREFIX__' | $(CROSS_PREFIX)gcc -E -P - 2>/dev/null)
ASFLAGS = -D_BasPag=$(USER_LABEL_PREFIX)_base -Wa,--register-prefix-optional

LDFLAGS=
LDSHARED=$(CC) -s -nostartfiles -Wl,--mprg-flags,0x0f

STATICLIB=libSDL.a
SHAREDLIB=sdl.slb
IMPORTLIB=libSDL.slb.a
ALLLIBS=$(STATICLIB) $(SHAREDLIB) $(IMPORTLIB)

ARFLAGS=rcs
LDSHAREDLIBC= -nostdlib -lgcc -lm
TAR=tar
SHELL=/bin/sh
EXE=

prefix = /usr
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
bindir = ${exec_prefix}/bin
sharedlibdir = ${prefix}/../mint/slb
includedir = ${prefix}/include
mandir = ${prefix}/share/man
man3dir = ${mandir}/man3
man5dir = ${mandir}/man5
pkgconfigdir = ${libdir}/pkgconfig

LIB_SRCS = \
	$(wildcard $(top_srcdir)/src/*.c) \
	\
	$(wildcard $(top_srcdir)/src/audio/*.c) \
	$(wildcard $(top_srcdir)/src/audio/mint/*.c) \
	$(wildcard $(top_srcdir)/src/audio/mint/*.S) \
	$(wildcard $(top_srcdir)/src/audio/disk/*.c) \
	$(wildcard $(top_srcdir)/src/audio/dummy/*.c) \
	\
	$(wildcard $(top_srcdir)/src/cdrom/*.c) \
	$(wildcard $(top_srcdir)/src/cdrom/mint/*.c) \
	\
	$(wildcard $(top_srcdir)/src/cpuinfo/*.c) \
	$(wildcard $(top_srcdir)/src/events/*.c) \
	$(wildcard $(top_srcdir)/src/file/*.c) \
	$(wildcard $(top_srcdir)/src/stdlib/*.c) \
	\
	$(wildcard $(top_srcdir)/src/joystick/*.c) \
	$(wildcard $(top_srcdir)/src/joystick/mint/*.c) \
	\
	$(wildcard $(top_srcdir)/src/thread/*.c) \
	\
	$(wildcard $(top_srcdir)/src/timer/*.c) \
	$(wildcard $(top_srcdir)/src/timer/mint/*.c) \
	\
	$(wildcard $(top_srcdir)/src/loadso/mint/SDL_sysloadso.c) \
	$(wildcard $(top_srcdir)/src/loadso/mint/ldg/*.c) \
	\
	$(wildcard $(top_srcdir)/src/video/*.c) \
	$(wildcard $(top_srcdir)/src/video/ataricommon/*.c) \
	$(wildcard $(top_srcdir)/src/video/ataricommon/*.S) \
	$(wildcard $(top_srcdir)/src/video/gem/*.c) \
	$(wildcard $(top_srcdir)/src/video/xbios/*.c) \
	$(wildcard $(top_srcdir)/src/video/dummy/*.c) \
	\
	$(empty)

ifneq ($(shell grep "SDL_PTHREAD_PTH 1" $(top_srcdir)/include/SDL_config.h),)
LIB_SRCS += $(wildcard $(top_srcdir)/src/thread/pth/*.c $(top_srcdir)/src/thread/generic/SDL_syssem.c)
else
LIB_SRCS += $(wildcard $(top_srcdir)/src/thread/generic/*.c)
endif

ALL_SRCS = \
	$(LIB_SRCS) \
	$(empty)

LIB_OBJS := $(subst .S,.o,$(subst .c,.o,$(notdir $(LIB_SRCS))))

PIC_OBJS = header.lo libinit.lo $(LIB_OBJS:.o=.lo)

GEN_IMP_SRCS := $(foreach num,$(shell seq 0 203),imp_$(num).c)
GEN_IMP_OBJS := $(GEN_IMP_SRCS:.c=.o)

IMP_OBJS = libptr.o libopen.o libclose.o slbload.o slbexec.o slbids.o $(GEN_IMP_OBJS)

all: static shared

static: $(STATICLIB)

shared: .genimpsources $(SHAREDLIB) $(IMPORTLIB)

.genimpsources: $(srcdir)/genimp.awk $(srcdir)/exports.h
	@if ! test -f $(top_srcdir)/include/SDL_config.h; then \
		echo "you must run configure first in the top level directory" >&2; \
		exit 1; \
	fi
	if ! grep -q slb/ $(top_srcdir)/include/SDL_config.h; then \
		sed -i '/#endif \/\* _SDL_config_h/i \
#ifndef __ASSEMBLER__\
#if defined(SDL_SLB)\
#include <slb/SDL.h>\
#endif\
#endif\
\
#if defined(__PUREC__) || defined(__AHCC__)\
#undef SDL_HAS_64BIT_TYPE\
#undef HAVE__LTOA\
#undef HAVE__UITOA\
#endif\
\
' $(top_srcdir)/include/SDL_config.h; \
	fi
	$(AWK) -f $(srcdir)/genimp.awk $(srcdir)/exports.h
	touch $@

define OBJ_template =
$(notdir $(basename $(1))).o: $(1)
	$(CC) $(CFLAGS) $(INCLUDES) $(CPPFLAGS) -c $$< -o $$@

$(notdir $(basename $(1))).lo: $(1)
	$(CC) $(SFLAGS) $(INCLUDES) $(CPPFLAGS) -c $$< -o $$@

endef

$(foreach file,$(ALL_SRCS) $(GEN_IMP_SRCS),$(eval $(call OBJ_template,$(file))))

$(GEN_IMP_SRCS): .genimpsources



check: test
	@:

test: all teststatic testshared

teststatic: static

testshared: shared

$(STATICLIB): $(LIB_OBJS)
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $^

$(IMPORTLIB): $(IMP_OBJS)
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $^

$(IMP_OBJS): $(srcdir)/lib_imp.h $(srcdir)/exports.h .genimpsources

libptr.o: $(srcdir)/libptr.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

libopen.o: $(srcdir)/libopen.c $(srcdir)/libstrct.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

libclose.o: $(srcdir)/libclose.c $(srcdir)/libstrct.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

slbload.o: $(srcdir)/slbload.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

slbexec.o: $(srcdir)/slbexec.S
	$(CC) $(CFLAGS) $(CPPFLAGS) $(ASFLAGS) -c $< -o $@

header.lo: $(srcdir)/header.S vernum.h $(srcdir)/exports.h $(srcdir)/symbols.h .genimpsources

vernum.h: $(top_srcdir)/include/SDL_version.h
	major=`grep 'define SDL_MAJOR_VERSION' $< | cut -d '	' -f 2` ; \
	minor=`grep 'define SDL_MINOR_VERSION' $< | cut -d '	' -f 2` ; \
	patch=`grep 'define SDL_PATCHLEVEL' $< | cut -d '	' -f 3` ; \
	version=`echo "$$major 1000 * $$minor 100 * $$patch + + p" | dc` ; \
	echo "#define SDL_VERSION_NUM $$version" > $@

$(SHAREDLIB): $(PIC_OBJS)
	$(LDSHARED) $(SFLAGS) $(CFLAGS) $^ -o $@ $(LDSHAREDLIBC) $(LDFLAGS)
	stripex $@




$(PIC_OBJS): $(srcdir)/libstrct.h



header.lo: $(srcdir)/header.S
	$(CC) $(SFLAGS) $(CPPFLAGS) $(ASFLAGS) -DSHAREDLIB=\"$(SHAREDLIB)\" -c -o $@ $<

libinit.lo: $(srcdir)/libinit.c
	$(CC) $(SFLAGS) $(CPPFLAGS) -c -o $@ $<


install-libs: all
	mkdir -p $(DESTDIR)$(exec_prefix)
	mkdir -p $(DESTDIR)$(libdir)
	mkdir -p $(DESTDIR)$(sharedlibdir)
	$(RM) $(DESTDIR)$(libdir)/$(STATICLIB)
	$(RM) $(DESTDIR)$(libdir)/$(IMPORTLIB)
	install -m 644 $(STATICLIB) $(IMPORTLIB) $(DESTDIR)$(libdir)
	$(RM) $(DESTDIR)$(sharedlibdir)/$(SHAREDLIB)
	echo "install $(SHAREDLIB) $(DESTDIR)$(sharedlibdir)"
	install -m 755 $(SHAREDLIB) $(DESTDIR)$(sharedlibdir)

install: install-libs
	mkdir -p $(DESTDIR)$(includedir)/SDL
	mkdir -p $(DESTDIR)$(includedir)/slb
	install -m 644 $(top_srcdir)/include/*.h $(DESTDIR)$(includedir)/SDL
	install -m 644 $(srcdir)/slb/SDL.h $(srcdir)/slb/slbids.h $(DESTDIR)$(includedir)/slb

install-strip: install
	$(STRIP) -g -x $(DESTDIR)$(libdir)/$(STATICLIB)
	$(RANLIB) $(DESTDIR)$(libdir)/$(STATICLIB)
	$(STRIP) -g -x $(DESTDIR)$(libdir)/$(IMPORTLIB)
	$(RANLIB) $(DESTDIR)$(libdir)/$(IMPORTLIB)

bindist: 
	$(MAKE) DESTDIR=/tmp/t/SDL PUREC=/windows/c/atari/pc/lib allbindist

allbindist:
	$(MAKE) clean
	$(MAKE) $(JOBS) CPU_CFLAGS="-m68020-60"
	$(MAKE) libdir="$(libdir)/m68020-60" sharedlibdir=$(sharedlibdir)/020 install-strip
	$(MAKE) clean
	$(MAKE) $(JOBS) CPU_CFLAGS="-mcpu=5475"
	$(MAKE) libdir="$(libdir)/m5475" sharedlibdir=$(sharedlibdir)/v4e install-strip
	$(MAKE) clean
	$(MAKE) $(JOBS) CPU_CFLAGS="-m68000"
	$(MAKE) libdir="$(libdir)" sharedlibdir=$(sharedlibdir)/000 install-strip
	install -m 755 $(SHAREDLIB) $(DESTDIR)$(sharedlibdir)
	$(MAKE) clean
	install -m 644 READ_SLB.txt $(DESTDIR)
	install -d $(DESTDIR)/purec/include/slb $(DESTDIR)/purec/include/SDL $(DESTDIR)/purec/lib/000 $(DESTDIR)/purec/lib/020
	install -m 644 $(top_srcdir)/include/*.h $(DESTDIR)/purec/include/SDL
	install -m 644 $(srcdir)/slb/SDL.h $(srcdir)/slb/slbids.h $(DESTDIR)/purec/include/slb
	cd $(DESTDIR)/purec/include && cr2nl -R -d .
	: install -m 644 $(PUREC)/000/sdl.lib $(PUREC)/000/sdlslb.lib $(DESTDIR)/purec/lib/000
	: install -m 644 $(PUREC)/020/sdl.lib $(PUREC)/020/sdlslb.lib $(DESTDIR)/purec/lib/020
	major=`grep 'define SDL_MAJOR_VERSION' $(top_srcdir)/include/SDL_version.h | cut -d '	' -f 2` ; \
	minor=`grep 'define SDL_MINOR_VERSION' $(top_srcdir)/include/SDL_version.h | cut -d '	' -f 2` ; \
	patch=`grep 'define SDL_PATCHLEVEL' $(top_srcdir)/include/SDL_version.h | cut -d '	' -f 3` ; \
	( cd $(DESTDIR)/..; rm -f sdl$${major}$${minor}$${patch}.zip; zip -r sdl$${major}$${minor}$${patch}.zip `basename $(DESTDIR)`; ); \
	( cd $(top_srcdir); git archive --prefix=sdl-$${major}.$${minor}.$${patch}-slb/ HEAD | bzip2 > $(DESTDIR)/../sdl-$${major}.$${minor}.$${patch}-slb.tar.bz2; )

mostlyclean: clean
clean:
	$(RM) *.o *.lo *.a *.slb $(GEN_IMP_SRCS)
	$(RM) .dirstamp .genimpsources nexports.h

maintainer-clean: distclean
distclean: clean
	-@$(RM) .DS_Store
	$(RM) vernum.h
