set terminal postscript eps color enhanced
set output "snr.eps"
set ylabel "Fraction"
set xlabel "SNR"
plot "histogram.dat" using 1:2 notitle with boxes lt -1
