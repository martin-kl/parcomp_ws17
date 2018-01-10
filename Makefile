CC = gcc-7

all: main

#make for saturn - with two steps it is not working:
main:
	$(CC) -Wall -O3 -fcilkplus -fopenmp -o main main.c generator.c quicksortC.c quicksortO.c quicksortS.c shared.c

mpi:
	mpicc  -std=c99 -O3 -o mpi quicksortM.c quicksortS.c shared.c generator.c

%.o: %.c
	$(CC) -Wall -O3 -c -o $@ $<


clean:
	rm -f generator.o shared.o main.o quicksortC.o quicksortS.o quicksortO.o main mpi
