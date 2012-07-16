dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(libwaveform, 0.1)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=libwaveform
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

LT_INIT

dnl API version
LIBWAVEFORM_API_VERSION=0.1
AC_SUBST(LIBWAVEFORM_API_VERSION)

PKG_CHECK_MODULES(LIBWAVEFORM, [gstreamer-1.0 glib-2.0 gtk+-3.0])

dnl check for gobject-introspection
GOBJECT_INTROSPECTION_CHECK([1.31.1])

AC_OUTPUT([
Makefile
src/libwaveform-0.1.pc
src/Makefile
po/Makefile.in])
