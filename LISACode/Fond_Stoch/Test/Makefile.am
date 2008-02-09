# $Id: Makefile.am,v 1.1 2006/06/09 09:22:23 petiteau Exp $
# Makefile.am

LIBS       = -llisacodebackground \
	@SLISAORBITS_LIBS@ \
	@SLISAMATHTOOLS_LIBS@

AM_LDFLAGS = -L../SRC \
	-L@SLISAORBITS_LIBDIR@ \
	-L@SLISAMATHTOOLS_LIBDIR@
			 
			 
INCLUDES   = -I../Include  \
	-I@SLISAORBITS_INCDIR@ \
	-I@SLISAMATHTOOLS_INCDIR@ \
	-I@SLISAGENERAL_INCDIR@

bin_PROGRAMS = \
		TestBackground \
		MakeCWDBSignal

TestBackground_SOURCES = LISACODE-TestBackground.cpp

MakeCWDBSignal_SOURCES = MakeCWDBSignal.cpp

# enf of Makefile.am