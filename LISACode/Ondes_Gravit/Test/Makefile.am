# $Id: Makefile.am,v 1.2 2006/03/15 16:58:14 petiteau Exp $
# Makefile.am

LIBS       = -llisacodegravwaves @SLISANOISE_LIBS@ @SLISAMATHTOOLS_LIBS@

AM_LDFLAGS = -L../SRC -L@SLISANOISE_LIBDIR@ -L@SLISAMATHTOOLS_LIBDIR@
			 
			 
INCLUDES   = -I../Include -I@SLISANOISE_INCDIR@ -I@SLISAMATHTOOLS_INCDIR@ -I@SLISAGENERAL_INCDIR@

bin_PROGRAMS = \
		TestGW \
		TestGWNewton2

TestGW_SOURCES = LISACODE-TestGW.cpp 
TestGWNewton2_SOURCES = LISACODE-TestGWNewton2.cpp 

# enf of Makefile.am
