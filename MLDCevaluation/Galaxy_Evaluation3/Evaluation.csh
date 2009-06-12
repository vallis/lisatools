#!/bin/csh

if ( ${#argv} != 3 ) then
echo "Not enough arguments: ./Evaluation.csh Brights.dat Recovered.dat Name"
exit
endif

gcc MLDC3_sort.c nrutil2.c -lm -o MLDC3_sort

gcc MLDC3_pairing.c template.c AEnoise.c dfour1.c nrutil2.c -lm -o MLDC3_pairing

# sort the bright file by frequency
./MLDC3_sort $1

# sort the entry by frequency
./MLDC3_sort $2

# compare recovered sources to brights file using noise weighted residual
# to do the pairing and full templates to compute the correlations
./MLDC3_pairing $1 $2 $3

cp $3_correlation_histograms.dat histogram.dat
gnuplot corr.gnu
cp correlation.eps $3_correlation.eps
cp $3_parameter_histograms.dat histogram.dat
gnuplot snr.gnu
mv snr.eps $3_snr.eps
gnuplot freq.gnu
mv freq.eps $3_freq.eps
gnuplot fdot.gnu
mv fdot.eps $3_fdot.eps
gnuplot theta.gnu
mv theta.eps $3_theta.eps
gnuplot phi.gnu
mv phi.eps $3_phi.eps
gnuplot amp.gnu
mv amp.eps $3_amp.eps
gnuplot inc.gnu
mv inc.eps $3_inc.eps
gnuplot pol.gnu
mv pol.eps $3_pol.eps
gnuplot phase.gnu
mv phase.eps $3_phase.eps

