#!/usr/bin/gnuplot

set logscale x 2;
set pointsize 2;
plot "cm2_plot.dat" using 1:2 with lines title "8kB",  \
	 "cm2_plot.dat" using 1:3 with lines title "8kB",  \
     "cm2_plot.dat" using 1:4 with lines title "16kB", \
     "cm2_plot.dat" using 1:5 with lines title "32kB", \
     "cm2_plot.dat" using 1:6 with lines title "64kB";

pause mouse
