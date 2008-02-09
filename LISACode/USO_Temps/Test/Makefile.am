# Makefile.am

LIBS       = -llisacodeusotime @SLISAMATHTOOLS_LIBS@

AM_LDFLAGS = -L../SRC -L@SLISAMATHTOOLS_LIBDIR@
			 
			 
INCLUDES   = -I../Include -I@SLISAGENERAL_INCDIR@  -I@SLISAMATHTOOLS_INCDIR@

bin_PROGRAMS = \
		TestUSOTime 

TestUSOTime_SOURCES = LISACODE-TestUSOTime.cpp 

# enf of Makefile.am