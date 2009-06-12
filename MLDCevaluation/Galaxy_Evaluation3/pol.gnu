set terminal postscript eps color enhanced
set output "pol.eps"
set ylabel "Fraction"
set xlabel "Polarization Error (radians)"
plot "histogram.dat" using 15:16 notitle with boxes lt -1
