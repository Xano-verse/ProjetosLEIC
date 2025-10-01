TARGETS = cm1
PAPILIB_ALAMEDA=/home/xanodeb/Documents/Ano\ Letivo\ 2025-26/1o\ Semestre/Organização\ de\ Computadores/Projeto\ Laboratoriais/Projeto1/lab1_kit/papi-7.2.0/src
#PAPILIB_ALAMEDA=/run/current-system/sw/lib/libpapi.so
#PAPILIB_TAGUS=/usr/local/lib/libpapi.so

all: $(TARGETS)

cm1: cm1.c
ifeq ($(shell test -e $(PAPILIB_ALAMEDA) && echo -n yes),yes)
	$(CC) $(CFLAGS) cm1.c $(PAPILIB_ALAMEDA) -o cm1
else
	$(CC) $(CFLAGS) cm1.c $(PAPILIB_TAGUS) -o cm1
endif

clean:
	rm -f $(TARGETS) *.o *.stderr *.stdout core *~
	rm *.out *.dat
	touch cm1_plot.dat
