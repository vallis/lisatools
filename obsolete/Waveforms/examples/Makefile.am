## Process this file with automake to produce Makefile.in
check_PROGRAMS = BBHGen EMRIGen

BBHGen_SOURCES = BBHGenerate.cc
BBHGen_LDADD = $(top_builddir)/src/liblisawpwavegen.la

EMRIGen_SOURCES = EMRIGenerate.cc
EMRIGen_LDADD = $(top_builddir)/src/liblisawpwavegen.la

INCLUDES = -I$(top_builddir)/include -I$(srcdir)/../include 

TESTS = BBHGen EMRIGen
TESTS_ENVIRONMENT = 1>/dev/null 2>/dev/null
CLEANFILES = *.dat

