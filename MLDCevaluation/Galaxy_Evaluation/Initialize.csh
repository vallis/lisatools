#!/bin/csh

if ( ${#argv} != 4 ) then
echo "Not enough arguments: ./Initialize.csh challenge.xml Full_key.txt Brights_file Key_Name"
exit
endif

# create a bright file from the full key
./bright_cut $2 $3

# sort the bright file by frequency
./MLDC2_sortkey $3

# identify sources in focus windows
./MLDC2_windows $3

# Apply the BIC code to establish baselines
./MLDC2_BIC_key $1 $3 $4

