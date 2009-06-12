set terminal postscript eps color enhanced
set output "amp.eps"
set ylabel "Fraction"
set xlabel "Log Amplitude Error"
plot "histogram.dat" using 11:12 notitle with boxes lt -1
