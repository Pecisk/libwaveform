## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIBWAVEFORM_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libwaveform.la


libwaveform_la_SOURCES = \
	waveformreader.c\
	waveformlevelreading.c

libwaveform_la_LDFLAGS = 

libwaveform_la_LIBADD = $(LIBWAVEFORM_LIBS)

include_HEADERS = \
	libwaveform.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libwaveform-0.1.pc

EXTRA_DIST = \
	libwaveform-0.1.pc.in