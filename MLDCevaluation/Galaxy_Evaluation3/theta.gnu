set terminal postscript eps color enhanced
set output "theta.eps"
set ylabel "Fraction"
set xlabel "Latitude Error (radians)"
plot "histogram.dat" using 7:8 notitle with boxes lt -1
