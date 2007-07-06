#!/bin/csh

if ( ${#argv} != 4 ) then
echo "Not enough arguments: ./Evaluation.csh challenge.xml Brights.dat Recovered.dat Name"
exit
endif

# sort the entry by frequency
./MLDC2_sortentry $3

# compare recovered sources to brights file using noise weighted residual
# to do the pairing and full templates to compute the correlations
./MLDC2_pairing $2 $3 $4

# compare recovered sources to brights file using doppler metric distance
# to do the pairing and estimate the correlations
./MLDC2_metric $2 $3 $4

#Use the BIC to test for over fitting of the data and produce residuals
./MLDC2_BIC $1 $3 $4

