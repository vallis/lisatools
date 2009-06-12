set terminal postscript eps color enhanced
set output "phase.eps"
set ylabel "Fraction"
set xlabel "Phase Error (radians)"
plot "histogram.dat" using 17:18 notitle with boxes lt -1
