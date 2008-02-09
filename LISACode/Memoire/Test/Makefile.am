# $Id: Makefile.am,v 1.2 2006/03/15 16:58:14 petiteau Exp $
# Makefile.am

LIBS       = -llisacodememory @SLISAMATHTOOLS_LIBS@

AM_LDFLAGS = -L../SRC -L@SLISAMATHTOOLS_LIBDIR@
			 
			 
INCLUDES   = -I../Include  -I@SLISAGENERAL_INCDIR@  -I@SLISAMATHTOOLS_INCDIR@

bin_PROGRAMS = \
		TestMemory 

TestMemory_SOURCES = LISACODE-TestMemory.cpp

# enf of Makefile.am