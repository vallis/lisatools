dnl Process this file with autoconf to produce a configure script.

AC_INIT([LISAWP],[0.1])
#AC_CONFIG_HEADERS([config.h])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL
#AC_PROG_RANLIB

AC_CHECK_PROGS(PYTHON, python2.3 python2.2 python2 python, echo)

AC_CHECK_LIB(m, sin)
AC_CHECK_LIB(z, main)
dnl Check for Dislin, this is not complete as library has different name according to version 
#AC_CHECK_LIB(dislin, main, , dislin="false")
#AC_CHECK_LIBS(dislnc, main, ,dislin="false")
dnl next was used for 
#  AC_CHECK_HEADERS(metaio.h, ,
#                   [AC_MSG_WARN(disabling metaio support)]
#                   [metaio="false"]
#                   [CPPFLAGS="$lal_pre_metaio_CPPFLAGS"])
#AC_CHECK_LIB(metaio, LALSnglInspiralTableFromLIGOLw, , lalmetaio="false")
# AC_SEARCH_LIBS(MetaioOpen, metaio dataflow, ,
#                 [AC_MSG_WARN(disabling metaio support)]
#                 [metaio="false"]
#                 [LIBS="$lal_pre_metaio_LIBS"])




#AC_SEARCH_LIBS(fftw_execute, fftw3, , [AC_MSG_ERROR(can't find double precision fftw3 library)], -lm)
#AC_SEARCH_LIBS(fftwf_execute, fftw3f, , [AC_MSG_ERROR(can't find single precision fftw3 library)], -lm)
#AC_CHECK_LIB(fftw3f, fftwf_execute_dft, , [AC_MSG_ERROR(could not find fftw3f library)], -lm)
#AC_CHECK_LIB(fftw3, fftw_execute_dft, , [AC_MSG_ERROR(could not find fftw3 library)], -lm)
AC_CHECK_LIB(gslcblas,main, , [AC_MSG_ERROR(Can't find library libgslcblas)])
AC_CHECK_LIB(gsl,gsl_strerror, , [AC_MSG_ERROR(Can't find library libgsl)])

AC_CHECK_HEADERS([gsl/gsl_fft_real.h])
#AC_CHECK_HEADERS(fftw3.h, , [AC_MSG_ERROR(could not find fftw3.h)])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([include/Makefile])
AC_CONFIG_FILES([examples/Makefile])
AC_OUTPUT

#([Makefile \
#           base/Makefile \
#           base/src/Makefile \
#           base/include/Makefile 
##           base/test/Makefile 
##           toolbox/Makefile \
##           toolbox/src/Makefile \
##           toolbox/include/Makefile \
##           pipelines/Makefile \
##           pipeline/src/Makefile \
##           pipeline/include/Makefile \
##           pipeline/test/Makefile \
##           waveforms/Makefile \
##           waveforms/src/Makefile \
##           waveforms/include/Makefile \
##           waveforms/test/Makefile \

