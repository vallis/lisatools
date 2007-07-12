#!/bin/sh
# Use of a while loop to read sources.txt file
cat sources.txt |   \
while read line
do
      ./galactic_barycenter $line
done

