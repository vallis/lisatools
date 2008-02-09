# $Id: Makefile.am,v 1.3 2006/03/15 16:58:14 petiteau Exp $
# Makefile.am

LIBS       = -llisacodeorbits @SLISAMATHTOOLS_LIBS@

AM_LDFLAGS = -L../SRC -L@SLISAMATHTOOLS_LIBDIR@
			 
			 
INCLUDES   = -I../Include -I@SLISAGENERAL_INCDIR@  -I@SLISAMATHTOOLS_INCDIR@

bin_PROGRAMS = \
		TestGeometry

TestGeometry_SOURCES = LISACODE-TestGeometry.cpp

# enf of Makefile.am