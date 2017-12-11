CC = gcc
CFLAGS = -Wall -g -std=c99 $(DEFS)

#OBJECTFILES = client.o server.o

.PHONY: all clean

all: sqsort

sqsort: sqsort.o generator.o
	gcc -o sqsort -O3 sqsort.o generator.o


%.o: %.c
		gcc -Wall -O3 -c -o $@ $<

#sqsort.o:
#	gcc -c -o sqsort.o sqsort.c
	
#generator.o:
#	gcc -c -o generator.o generator.c

clean:
	rm -f sqsort sqsort.o generator.o
