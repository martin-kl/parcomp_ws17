CC = gcc
CFLAGS = -Wall -g -std=c99 $(DEFS)

#OBJECTFILES = client.o server.o

.PHONY: all clean

all: sqsort

sqsort: 
	gcc -o sqsort -O3 sqsort.c
	
clean:
	rm -f sqsort sqsort.o
	#rm -f $(OBJECTFILES) client server
