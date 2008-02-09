# $Id: Makefile.am,v 1.2 2006/03/15 16:58:14 petiteau Exp $
# Makefile.am

LIBS       = -llisacodenoise @SLISAMATHTOOLS_LIBS@

AM_LDFLAGS = -L../SRC -L@SLISAMATHTOOLS_LIBDIR@
			 
			 
INCLUDES   = -I../Include -I@SLISAGENERAL_INCDIR@  -I@SLISAMATHTOOLS_INCDIR@

bin_PROGRAMS = \
		TestNoise

TestNoise_SOURCES = LISACODE-TestNoise.cpp

# enf of Makefile.am