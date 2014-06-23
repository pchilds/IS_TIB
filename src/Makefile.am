## Process this file with automake to produce Makefile.in
AM_CFLAGS=$(IS_CFLAGS)
AM_CPPFLAGS= -DLOCALEDIR=\"$(localedir)\" -DPACKAGE=\"$(PACKAGE)\"
bin_PROGRAMS=InvScatt
InvScatt_SOURCES=		\
	main.c			\
	main.h			\
	util.c			\
	util.h			\
	data.c			\
	data.h			\
	proc.c			\
	proc.h			\
	disp.c			\
	disp.h
InvScatt_LDADD=$(IS_LIBS) -lm
MAINTAINERCLEANFILES=Makefile.in
