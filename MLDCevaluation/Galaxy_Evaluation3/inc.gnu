set terminal postscript eps color enhanced
set output "inc.eps"
set ylabel "Fraction"
set xlabel "Inclination Error (radians)"
plot "histogram.dat" using 13:14 notitle with boxes lt -1
