CC = gcc-7
CFLAGS = -Wall -g -std=c99 $(DEFS)

#OBJECTFILES = client.o server.o

.PHONY: all clean

all: main

main: generator.o main.o quicksortC.o quicksortS.o quicksortO.o
	$(CC) -fcilkplus -fopenmp -Wall -o $@ -O3 $^

quicksortC.o: quicksortC.c
	$(CC) -Wall -O3 -fcilkplus -c -o $@ $<

quicksortO.o: quicksortO.c
	$(CC) -Wall -O3 -fopenmp -c -o $@ $<

#just for Testing:
testscan : testscan.o generator.o
	$(CC) -Wall -fcilkplus -O3 testscan.o generator.o -o $@
testscan.o: testscan.c
	$(CC) -Wall -O3 -fcilkplus -c -o $@ $<


%.o: %.c
	$(CC) -Wall -O3 -c -o $@ $<


clean:
	rm -f generator.o main.o quicksortC.o quicksortS.o quicksortO.o main
