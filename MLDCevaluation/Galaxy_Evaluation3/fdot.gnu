set terminal postscript eps color enhanced
set output "fdot.eps"
set ylabel "Fraction"
set xlabel "Frequency Derivative Error (bins)"
plot "histogram.dat" using 5:6 notitle with boxes lt -1
