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

-include $(INTROSPECTION_MAKEFILE)
  INTROSPECTION_GIRS =
  INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
  INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

  if HAVE_INTROSPECTION
  introspection_sources = $(libwaveform_la_SOURCES)

  Waveform-0.1.gir: libwaveform.la
  Waveform_0_1_gir_INCLUDES = GObject-2.0 Gtk-3.0 Gst-1.0
  Waveform_0_1_gir_CFLAGS = $(INCLUDES)
  Waveform_0_1_gir_LIBS = libwaveform.la
  Waveform_0_1_gir_FILES = $(introspection_sources)
  INTROSPECTION_GIRS += Waveform-0.1.gir

  girdir = $(datadir)/gir-1.0
  gir_DATA = $(INTROSPECTION_GIRS)

  typelibdir = $(libdir)/girepository-1.0
  typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

  CLEANFILES += $(gir_DATA) $(typelib_DATA)
  endif