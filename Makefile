CC = gcc
CFLAGS = -Wall -g -std=c99 $(DEFS)

#OBJECTFILES = client.o server.o

.PHONY: all clean

all: main cilk openmp

main: sqsort.o generator.o main.o cqsort.o
	gcc -Wall -fcilkplus -o $@ -O3 sqsort.o generator.o main.o cqsort.o
	#$(CC) -Wall -fcilkplus -o main -O3 sqsort.o generator.o main.o cqsort.o

cilk: sqsort.o cqsort.o cqmain.o
	$(CC) -Wall -fcilkplus -O3 cqsort.o sqsort.o cqmain.o -o cilk
	#$(CC) -Wall -fcilkplus -O3 cqsort.o sqsort.o cqmain.o -o cilk

openmp:
	$(CC)	-Wall -fopenmp -O3 ompquick.c generator.c sqsort.c -o openmp


#just for Testing:
testscan : testscan.o generator.o
	$(CC) -Wall -fcilkplus -O3 testscan.o generator.o -o $@
testscan.o: testscan.c
	$(CC) -Wall -O3 -fcilkplus -c -o $@ $<


#needed for cilk:
cqsort.o: cqsort.c
	$(CC) -Wall -O3 -fcilkplus -c -o $@ $<
cqmain.o: cqmain.c
	$(CC) -Wall -O3 -fcilkplus -c -o $@ $<


%.o: %.c
	$(CC) -Wall -O3 -c -o $@ $<


clean:
	rm -f sqsort.o cqsort.o generator.o main.o main cqmain.o cilk testscan.o testscan openmp 
