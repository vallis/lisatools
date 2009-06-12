set terminal postscript eps color enhanced
set output "correlation.eps"
set xrange [0.5:1]
set ylabel "# Sources"
set xlabel "Correlation"
plot "histogram.dat" using 1:2 notitle with boxes lt -1
