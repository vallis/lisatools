# $Id: configure.ac,v 1.6 2006/09/18 14:34:26 petiteau Exp $
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_INIT(SLisa, 1.4, petiteau@apc.univ-paris7.fr)
AC_CONFIG_AUX_DIR([.])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(lisacode,1.4)


# Checks for programs.
AC_PROG_INSTALL
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([floor pow sqrt])

# Checks for libraries.
AC_CHECK_LIB([dl], [dlerror])
AC_CHECK_LIB([m], [cos])


### lisacodegeneral ###
# AC_MSG_CHECKING(lisacodegeneral)
# test x$SLISAGENERAL_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodegeneral! Install it first!])

AC_SUBST(SLISAGENERAL_INCDIR,$PWD/Generalites/Include)


### lisacodemathtools ###
# AC_MSG_CHECKING(lisacodemathtools)
# test x$SLISAMATHTOOLS_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodemathtools! Install it first!])

AC_SUBST(SLISAMATHTOOLS_SYS,$PWD/Outils_Math)
AC_SUBST(SLISAMATHTOOLS_LIBS,["-L$PWD/Outils_Math/SRC -llisacodemathtools"])
AC_SUBST(SLISAMATHTOOLS_INCDIR,$PWD/Outils_Math/Include)
AC_SUBST(SLISAMATHTOOLS_LIBDIR,$PWD/Outils_Math/SRC)


### lisacodenoise ###
# AC_MSG_CHECKING(lisacodenoise)
# test x$SLISANOISE_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodenoise! Install it first!])

AC_SUBST(SLISANOISE_SYS,$PWD/Bruits)
AC_SUBST(SLISANOISE_LIBS,["-L$PWD/Bruits/SRC -llisacodenoise"])
AC_SUBST(SLISANOISE_INCDIR,$PWD/Bruits/Include)
AC_SUBST(SLISANOISE_LIBDIR,$PWD/Bruits/SRC)


### lisacodegravwaves ###
# AC_MSG_CHECKING(lisacodegravwaves)
# test x$SLISAGRAVWAVES_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodegravwaves! Install it first!])

AC_SUBST(SLISAGRAVWAVES_SYS,$PWD/Ondes_Gravit)
AC_SUBST(SLISAGRAVWAVES_LIBS,["-L$PWD/Ondes_Gravit/SRC -llisacodegravwaves"])
AC_SUBST(SLISAGRAVWAVES_INCDIR,$PWD/Ondes_Gravit/Include)
AC_SUBST(SLISAGRAVWAVES_LIBDIR,$PWD/Ondes_Gravit/SRC)


### lisacodeorbits ###
# AC_MSG_CHECKING(lisacodeorbits)
# test x$SLISAORBITS_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodeorbits! Install it first!])

AC_SUBST(SLISAORBITS_SYS,$PWD/Orbitographie)
AC_SUBST(SLISAORBITS_LIBS,["-L$PWD/Orbitographie/SRC -llisacodeorbits"])
AC_SUBST(SLISAORBITS_INCDIR,$PWD/Orbitographie/Include)
AC_SUBST(SLISAORBITS_LIBDIR,$PWD/Orbitographie/SRC)


### lisacodebackground ###
# AC_MSG_CHECKING(lisacodebackground)
# test x$SLISABACKGROUND_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodenoise! Install it first!])

AC_SUBST(SLISABACKGROUND_SYS,$PWD/Fond_Stoch)
AC_SUBST(SLISABACKGROUND_LIBS,["-L$PWD/Fond_Stoch/SRC -llisacodebackground"])
AC_SUBST(SLISABACKGROUND_INCDIR,$PWD/Fond_Stoch/Include)
AC_SUBST(SLISABACKGROUND_LIBDIR,$PWD/Fond_Stoch/SRC)


### lisacodeusotime ###
# AC_MSG_CHECKING(lisacodeusotime)
# test x$SLISAUSOTIME_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodeusotime! Install it first!])

AC_SUBST(SLISAUSOTIME_SYS,$PWD/USO_Temps)
AC_SUBST(SLISAUSOTIME_LIBS,["-L$PWD/USO_Temps/SRC -llisacodeusotime"])
AC_SUBST(SLISAUSOTIME_INCDIR,$PWD/USO_Temps/Include)
AC_SUBST(SLISAUSOTIME_LIBDIR,$PWD/USO_Temps/SRC)


### lisacodememory ###
# AC_MSG_CHECKING(lisacodememory)
# test x$SLISAMEMORY_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodememory! Install it first!])

AC_SUBST(SLISAMEMORY_SYS,$PWD/Memoire)
AC_SUBST(SLISAMEMORY_LIBS,["-L$PWD/Memoire/SRC -llisacodememory"])
AC_SUBST(SLISAMEMORY_INCDIR,$PWD/Memoire/Include)
AC_SUBST(SLISAMEMORY_LIBDIR,$PWD/Memoire/SRC)


### lisacodeinputdata ###
# AC_MSG_CHECKING(lisacodeinputdata)
# test x$SLISAINPUTDATA_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodeinputdata! Install it first!])

AC_SUBST(SLISAINPUTDATA_SYS,$PWD/Input_data)
AC_SUBST(SLISAINPUTDATA_LIBS,["-L$PWD/Input_data/SRC -llisacodeinputdata"])
AC_SUBST(SLISAINPUTDATA_INCDIR,$PWD/Input_data/Include)
AC_SUBST(SLISAINPUTDATA_LIBDIR,$PWD/Input_data/SRC)


### lisacodedetector ###
# AC_MSG_CHECKING(lisacodedetector)
# test x$SLISADETECTOR_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodedetector! Install it first!])

AC_SUBST(SLISADETECTOR_SYS,$PWD/Detecteur)
AC_SUBST(SLISADETECTOR_LIBS,["-L$PWD/Detecteur/SRC -llisacodedetector"])
AC_SUBST(SLISADETECTOR_INCDIR,$PWD/Detecteur/Include)
AC_SUBST(SLISADETECTOR_LIBDIR,$PWD/Detecteur/SRC)


### lisacodetdi ###
# AC_MSG_CHECKING(lisacodetdi)
# test x$SLISATDI_ROOT != x && ok=yes || ok=no
# AC_MSG_RESULT($ok)
# test $ok != no || AC_MSG_ERROR([No lisacodetdi! Install it first!])

AC_SUBST(SLISATDI_SYS,$PWD/TDI)
AC_SUBST(SLISATDI_LIBS,["-L$PWD/TDI/SRC -llisacodetdi"])
AC_SUBST(SLISATDI_INCDIR,$PWD/TDI/Include)
AC_SUBST(SLISATDI_LIBDIR,$PWD/TDI/SRC)


# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile
				Outils_Math/Makefile
				Outils_Math/SRC/Makefile
				Outils_Math/Test/Makefile
				Bruits/Makefile
				Bruits/SRC/Makefile
				Bruits/Test/Makefile
				Ondes_Gravit/Makefile
				Ondes_Gravit/SRC/Makefile
				Ondes_Gravit/Test/Makefile
				Orbitographie/Makefile
				Orbitographie/SRC/Makefile
				Orbitographie/Test/Makefile
				Fond_Stoch/Makefile
				Fond_Stoch/SRC/Makefile
				Fond_Stoch/Test/Makefile
				USO_Temps/Makefile
				USO_Temps/SRC/Makefile
				USO_Temps/Test/Makefile
				Memoire/Makefile
				Memoire/SRC/Makefile
				Memoire/Test/Makefile
				Input_data/Makefile
				Input_data/SRC/Makefile
				Input_data/Test/Makefile
				Detecteur/Makefile
				Detecteur/SRC/Makefile
				Detecteur/Test/Makefile
				TDI/Makefile
				TDI/SRC/Makefile
				TDI/Test/Makefile
				Main/Makefile
				Main/SRC/Makefile
				Main/Test/Makefile
		])
AC_OUTPUT