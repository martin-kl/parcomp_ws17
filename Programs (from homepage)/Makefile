mpihello:
	mpicc -o mpihello -O3 mpihello.c

openmphello:
	gcc -Wall -fopenmp -o openmphello -O3 openmphello.c

pthreadshello:
	gcc -Wall -o pthreadshello -O3 pthreadshello.c -pthread	

cilkhello:
	gcc -Wall -fcilkplus -O3 cilkhello.c -o cilkhello

cilkqsort:
	gcc -Wall -fcilkplus -O3 cilkqsort.c -o cqs

ompquick:
	gcc -Wall -fopenmp -O3 ompqsort.c -o oqs

clean:
	rm -rf mpihello openmphello pthreadshello cilkhello cqs oqs
