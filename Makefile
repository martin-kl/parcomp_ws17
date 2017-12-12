CC = gcc
CFLAGS = -Wall -g -std=c99 $(DEFS)

#OBJECTFILES = client.o server.o

.PHONY: all clean

all: main cilkMain

main: sqsort.o generator.o main.o cqsort.o
	gcc -Wall -fcilkplus -o $@ -O3 sqsort.o generator.o main.o cqsort.o
	#gcc -Wall -fcilkplus -o main -O3 sqsort.o generator.o main.o cqsort.o

cilkMain: sqsort.o cqsort.o cqmain.o
	gcc -Wall -fcilkplus -O3 cqsort.o sqsort.o cqmain.o -o cilkMain
	#gcc -Wall -fcilkplus -O3 cqsort.o sqsort.o cqmain.o -o cilkMain

cqsort.o: cqsort.c
	gcc -Wall -O3 -fcilkplus -c -o $@ $<
cqmain.o: cqmain.c
	gcc -Wall -O3 -fcilkplus -c -o $@ $<

%.o: %.c
	gcc -Wall -O3 -c -o $@ $<


clean:
	rm -f sqsort.o cqsort.o generator.o main.o main cqmain.o cilkMain
