set terminal postscript eps color enhanced
set output "freq.eps"
set ylabel "Fraction"
set xlabel "Frequency Error (bins)"
plot "histogram.dat" using 3:4 notitle with boxes lt -1
