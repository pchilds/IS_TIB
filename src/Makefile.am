## Process this file with automake to produce Makefile.in
AM_CFLAGS=$(IS_CFLAGS)
AM_CPPFLAGS= -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(PACKAGE)\"
bin_PROGRAMS=InvScatt
InvScatt_SOURCES=	\
	is-tib0-1-0.c	\
	plotlinear0-1-0.c	\
	is-tib0-1-0.h	\
	plotlinear0-1-0.h
InvScatt_LDADD=$(IS_LIBS)
