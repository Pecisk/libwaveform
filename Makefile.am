SUBDIRS = src po

libwaveformdocdir = ${prefix}/doc/libwaveform
libwaveformdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(libwaveformdoc_DATA) \
	$(INTLTOOL_FILES)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(libwaveformdocdir)
