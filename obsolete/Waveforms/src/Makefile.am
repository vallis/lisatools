### Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = liblisawpwavegen.la

WAVEFORMSOURCES = BaseIntegr.cc\
		  Matrix.cc\
		  IndexSet.cc\
		  Constants.cc\
		  BBHChallenge1.cc\
		  AKWaveform.cc

liblisawpwavegen_la_SOURCES = $(WAVEFORMSOURCES)

INCLUDES = -I$(top_builddir)/include \
           -I$(srcdir)/../include

