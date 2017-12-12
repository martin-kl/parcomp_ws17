CC = gcc
CFLAGS = -Wall -g -std=c99 $(DEFS)

#OBJECTFILES = client.o server.o

.PHONY: all clean

all: main cilkqsort

main: sqsort.o generator.o main.o
	gcc -o mainSeq -O3 sqsort.o generator.o main.o

cilkqsort: sqsort.o cilkqsort.o
	gcc -Wall -fcilkplus -O3 cilkqsort.o sqsort.o -o cilkqs

cilkqsort.o: cilkqsort.c
	gcc -Wall -O3 -fcilkplus -c -o $@ $<

%.o: %.c
	gcc -Wall -O3 -c -o $@ $<

#sqsort.o:
#	gcc -c -o sqsort.o sqsort.c
	
#generator.o:
#	gcc -c -o generator.o generator.c

clean:
	rm -f sqsort.o generator.o cilkqsort.o main.o mainSeq cilkqs
