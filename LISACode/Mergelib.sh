#!/bin/sh

# Mergelib.sh
# LISACode_1_4
#
# Created by Antoine Petiteau on 04/07/09.
# Copyright 2009 Max-Planck-Institut für Gravitationsphysik - Albert Einstein Institute. All rights reserved.

InstallPath="$HOME/Applications"

## Copy headers
cp */*/*.h "$InstallPath/include/."

## Merge libraries
rm -r tmplib2 lib
mkdir lib
cp */SRC/*.a lib/.
mkdir tmplib2
cd tmplib2
for libfile in ../lib/*.a
do ar -x $libfile 
done
ar cru ../lib/liblisacode.a *.o
ranlib ../lib/liblisacode.a
cd ..
rm -r tmplib2
rm "$InstallPath/lib/liblisacode.a"
cp lib/liblisacode.a "$InstallPath/lib/."
ranlib "$InstallPath/lib/liblisacode.a"
echo "--> Success of merging and installation in $InstallPath."

