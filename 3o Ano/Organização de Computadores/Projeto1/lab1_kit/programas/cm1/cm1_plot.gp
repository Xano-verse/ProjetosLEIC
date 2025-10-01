#!/usr/bin/gnuplot

set logscale x 2;
set pointsize 2;
plot "cm1_plot.dat" using 1:2 with linespoints title "8kB",  \
     "cm1_plot.dat" using 1:3 with linespoints title "16kB", \
     "cm1_plot.dat" using 1:4 with linespoints title "32kB", \
     "cm1_plot.dat" using 1:5 with linespoints title "64kB";

pause mouse
