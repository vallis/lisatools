# $Id: Makefile.am,v 1.4 2006/09/18 14:34:28 petiteau Exp $
# Makefile.am

INCLUDES = \
	-I../Include \
	-I@SLISAUSOTIME_INCDIR@ \
	-I@SLISANOISE_INCDIR@ \
	-I@SLISABACKGROUND_INCDIR@ \
	-I@SLISAORBITS_INCDIR@ \
	-I@SLISAGRAVWAVES_INCDIR@ \
	-I@SLISAMATHTOOLS_INCDIR@ \
	-I@SLISAGENERAL_INCDIR@

lib_LIBRARIES = liblisacodeinputdata.a

liblisacodeinputdata_a_SOURCES = \
	LISACODE-ConfigSim.cpp \
	ezxml.c

# end of Makefile.am