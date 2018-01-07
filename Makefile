CC = gcc-7
CFLAGS = -Wall -g -std=c99 $(DEFS)

#OBJECTFILES = client.o server.o

.PHONY: all clean

all: main

#make for saturn - with two steps it is not working:
main:
	$(CC) -Wall -O3 -fcilkplus -fopenmp -o main main.c generator.c quicksortC.c quicksortO.c quicksortS.c shared.c -O3 -Wall

mpi:
	mpicc  -std=c99 -O0 -g -o mpi quicksortM.c quicksortS.c shared.c generator.c
	#this should be the call afterwars - the upper call is just for debugging
	#mpicc  -std=c99 -O3 -o mpi quicksortM.c quicksortS.c shared.c generator.c

#main: generator.o main.o quicksortC.o quicksortS.o quicksortO.o shared.o
#	$(CC) -fcilkplus -fopenmp -Wall -o $@ -O3 $^

#quicksortC.o: quicksortC.c
#	$(CC) -Wall -O3 -fcilkplus -c -o $@ $<

#quicksortO.o: quicksortO.c
#	$(CC) -Wall -O3 -fopenmp -c -o $@ $<

#just for Testing:
testscan : testscan.o generator.o
	$(CC) -Wall -fcilkplus -O3 testscan.o generator.o -o $@
testscan.o: testscan.c
	$(CC) -Wall -O3 -fcilkplus -c -o $@ $<


%.o: %.c
	$(CC) -Wall -O3 -c -o $@ $<


clean:
	rm -f generator.o shared.o main.o quicksortC.o quicksortS.o quicksortO.o main mpi
