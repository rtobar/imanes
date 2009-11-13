bin_PROGRAMS = imanes

imanes_SOURCES = \
     apu.c \
     cnrom.c \
     common.c \
     clock.c \
     cpu.c \
     frame_control.c \
     gui.c \
     imaconfig.c \
     instruction_set.c \
     loop.c \
     main.c \
     mapper.c \
     mmc1.c \
     mmc3.c \
     nrom.c \
     pad.c \
     palette.c \
     parse_file.c \
     platform.c \
     ppu.c \
     screen.c \
     screenshot.c \
     sram.c \
     states.c \
     unrom.c \
     $(top_srcdir)/include/apu.h \
     $(top_srcdir)/include/clock.h \
     $(top_srcdir)/include/cnrom.h \
     $(top_srcdir)/include/common.h \
     $(top_srcdir)/include/cpu.h \
     $(top_srcdir)/include/debug.h \
     $(top_srcdir)/include/frame_control.h \
     $(top_srcdir)/include/gui.h \
     $(top_srcdir)/include/imaconfig.h \
     $(top_srcdir)/i18n.h \
     $(top_srcdir)/include/instruction_set.h \
     $(top_srcdir)/include/loop.h \
     $(top_srcdir)/include/mapper.h \
     $(top_srcdir)/include/mmc1.h \
     $(top_srcdir)/include/mmc3.h \
     $(top_srcdir)/include/nrom.h \
     $(top_srcdir)/include/pad.h \
     $(top_srcdir)/include/palette.h \
     $(top_srcdir)/include/parse_file.h \
     $(top_srcdir)/include/platform.h \
     $(top_srcdir)/include/ppu.h \
     $(top_srcdir)/include/screen.h \
     $(top_srcdir)/include/screenshot.h \
     $(top_srcdir)/include/sram.h \
     $(top_srcdir)/include/states.h \
     $(top_srcdir)/include/unrom.h

imanes_LDFLAGS = $(LIBINTL)

AM_CPPFLAGS = -I$(top_srcdir)/include -Wall -O3 -pedantic

DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
