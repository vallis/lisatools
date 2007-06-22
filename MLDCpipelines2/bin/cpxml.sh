#!/bin/sh

cat $1.xml | sed "s/$1/$2/g" > $2.xml
cp $1-0.bin $2-0.bin
