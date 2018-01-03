#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "generator.h"
#include "sorts.h"
#include "shared.h"


void generateArray(int *a, int s, int n, unsigned seed) {
  //generate array a:
  if (s==0) {
    generateIntPeriodicNumbers(a, n);
  } else if (s==1) {
    generateIntSameNumbers(a, n, 27);
  }else if (s==2) {
    generateIntAscendingNumbers(a, n);
  }else if (s==3) {
    generateIntDescendingNumbers(a, n);
  } else if (s == 4) {
    if(seed != 0)
      generateIntRandomNumbersWithSeed(a, n, 0, 100, seed);
    else
      generateIntRandomNumbers(a, n, 0, 100);
  }else {
    printf("invalid input for type, only 0-4 is valid\n");
    exit(0);
  }
}

void assertSorted(int * a, int n) {
  for (int i=0; i<n-1; i++) assert(a[i]<=a[i+1]);
}

int main(int argc, char *argv[]) {
  int calls = 1;
  double start, stop;
  int s = 0;
  int n = 1;
  unsigned seed = 0;
  int threads = 1;

  void (*implementation) (int*, int, int);

  for (int i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='t') i++,sscanf(argv[i],"%d",&threads); //number of threads
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
    if (argv[i][1]=='c') i++,sscanf(argv[i],"%d",&calls);
    if (argv[i][1]=='a') {
      i++;
      if (*argv[i] == 'o') {
        printf("Using OpenMP.\n");
        implementation = &quicksortO;
      } else if (*argv[i] == 'c') {
        printf("Using Cilk.\n");
        implementation = &quicksortC;
      } else if (*argv[i] == 'm') {
        printf("Not yet implemented. Exiting.\n");
        //implementation = &quicksortM;
      }
    }
  }
  if (implementation == NULL) {
    printf("Specify implementation via -a [o|c|m].");
    exit(1);
  }

  printf("Executing with: -n %i -s %i -t %i -S %i -c %i\n", n, s, threads, seed, calls);

  int * a = (int*)malloc(n*sizeof(int));



  printf("sorting times with parallel algorithm:\n");
  for(int i = 0; i < calls; i++) {
    generateArray(a, s, n, seed);
    start = mytime();
    implementation(a, n, threads);
    stop = mytime();
    assertSorted(a, n);
    printf(" > %f\n", stop-start);
  }

  generateArray(a, s, n, seed);

  //call sequential algorithm
  printf("start sequential algorithm for comparison...\n");
  start = mytime();
  quicksortS(a, 0, n-1);
  assertSorted(a, n);
  stop = mytime();
  printf("time for sequential algorithm: %.5f\n", (stop-start));
  printf("\n");

  free(a);
  return 0;
}
