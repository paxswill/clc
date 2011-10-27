CFLAGS:=-I/usr/local/cuda/include -Iclu/include -Wall -pedantic -std=c99 -O3
LDFLAGS:=-lOpenCL

CC:=gcc

all: clc

clc: clc.c clu.o
	$(CC) $(CFLAGS) $(LDFLAGS) clu.o clc.c -o clc

clu.o: clu/include/clu.h clu/clu.c
	$(CC) $(CFLAGS) -c clu/clu.c -o clu.o

clean:
	rm clu.o
	rm clc
