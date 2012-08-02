AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(LIBWAVEFORM_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

lib_LTLIBRARIES = libwaveform-@LIBWAVEFORM_API_VERSION@.la


libwaveform_@LIBWAVEFORM_API_VERSION@_la_SOURCES = \
	waveformreader.c\
	waveformlevelreading.c\
    waveformdata.c\
    waveformwidget.c

libwaveform_@LIBWAVEFORM_API_VERSION@_la_LDFLAGS = 

libwaveform_@LIBWAVEFORM_API_VERSION@_la_LIBADD = $(LIBWAVEFORM_LIBS)

include_HEADERS = \
	waveform.h\
    waveformreader.h\
    waveformlevelreading.h\
    waveformdata.h\
    waveformwidget.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libwaveform.pc

EXTRA_DIST = \
	libwaveform.pc.in

INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = $(libwaveform_@LIBWAVEFORM_API_VERSION@_la_SOURCES)
INTROSPECTION_GIRS = Waveform-@LIBWAVEFORM_API_VERSION@.gir

gir_headers=$(patsubst %,$(srcdir)/%, $(include_HEADERS))
gir_sources=$(patsubst %,$(srcdir)/%, $(libwaveform_@LIBWAVEFORM_API_VERSION@_la_SOURCES))

Waveform-@LIBWAVEFORM_API_VERSION@.gir: $(INTROSPECTION_SCANNER) libwaveform-@LIBWAVEFORM_API_VERSION@.la
	$(AM_V_GEN) $(INTROSPECTION_SCANNER) --verbose --warn-all --namespace=Waveform \
		--nsversion=@LIBWAVEFORM_API_VERSION@ \
		--warn-all \
		-I$(top_srcdir)/src \
		-I$(top_builddir)/src \
		-DIN_GOBJECT_INTROSPECTION=1 \
		--c-include='src/waveform.h' \
		--library=libwaveform-@LIBWAVEFORM_API_VERSION@.la \
		--include=GLib-2.0 \
		--include=GObject-2.0 \
		--include=Gst-1.0 \
		--include=Gtk-3.0 \
		--libtool="$(top_builddir)/libtool" \
		--pkg glib-2.0 \
		--pkg gobject-2.0 \
		--pkg gmodule-no-export-2.0 \
		--pkg gthread-2.0 \
		--pkg gstreamer-1.0 \
		--pkg-export waveform-@LIBWAVEFORM_API_VERSION@ \
		--output $@ \
		$(gir_headers) \
		$(gir_sources)

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)


%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(AM_V_GEN)$(INTROSPECTION_COMPILER) \
		--includedir=$(srcdir)/lib \
		--includedir=$(builddir)/lib \
		$(INTROSPECTION_COMPILER_OPTS) $< -o $(@F)

CLEANFILES = $(gir_DATA) $(typelib_DATA)

endif