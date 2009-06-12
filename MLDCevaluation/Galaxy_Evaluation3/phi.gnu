set terminal postscript eps color enhanced
set output "phi.eps"
set ylabel "Fraction"
set xlabel "Latitude Error (radians)"
plot "histogram.dat" using 9:10 notitle with boxes lt -1
