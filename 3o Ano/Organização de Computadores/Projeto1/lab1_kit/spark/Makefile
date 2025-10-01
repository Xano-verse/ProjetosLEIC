CC = gcc
CFLAGS =  -O3
LDFLAGS = -lrt
TARGET = spark

all: $(TARGET)
	
$(TARGET): spark.c
	$(CC) $(CFLAGS) spark.c -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGETS) *.o *.stderr *.stdout core *~
	rm *.out *.dat
	touch spark_plot.dat
