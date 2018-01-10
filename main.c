#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

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

void(*getFunction(char c)) (int*, int, int) {
  if (c == 'o') {
    printf("using OpenMP.\n");
    return &quicksortO;
  } else if (c == 'c') {
    printf("using Cilk.\n");
    return &quicksortC;
  } else if (c == 'C') {
    printf("using Cilk2.\n");
    return &quicksortC2;
  } else if (c == 'm') {
    printf("Not yet implemented. Exiting.\n");
    //implementation = &quicksortM;
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  int calls = 1;
  double start, stop;
  int s = 0;
  int n = 1;
  unsigned seed = 0;
  int threads = 1;
  char alg = 0;
  char Alg = 0;

  void (*implementation) (int*, int, int) = NULL;
  void (*compare) (int*, int, int) = NULL;

  for (int i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='t') i++,sscanf(argv[i],"%d",&threads); //number of threads
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
    if (argv[i][1]=='c') i++,sscanf(argv[i],"%d",&calls);
    if (argv[i][1]=='a') {
      i++;
      implementation = getFunction(*argv[i]);
      alg = *argv[i];
    }
    if (argv[i][1]=='A') {
      i++;
      printf("for comparison: ");
      Alg = *argv[i];
      compare = getFunction(*argv[i]);
    }
  }
  if (implementation == NULL) {
    printf("Specify implementation via -a [o|c|m].\n");
    exit(1);
  }

  printf("Executing with: -n %i -s %i -t %i -S %i -c %i -a %c -A %c\n", n, s, threads, seed, calls, alg, Alg);
  int * a = (int*)malloc(n*sizeof(int));

  double times[calls];
  double best1 = DBL_MAX;
  double worst1 = 0;
  double mean1 = 0;
  printf("\nsorting times with parallel algorithm:\n");
  for(int i = 0; i < calls; i++) {
    generateArray(a, s, n, seed);
    start = mytime();
    implementation(a, n, threads);
    stop = mytime();
    times[i] = stop-start;
    if(times[i] > worst1) worst1 = times[i];
    if(times[i] < best1) best1 = times[i];
    mean1 += times[i];
    //printf(" > %f\n", stop-start);
  }
  //only assert last solution
  assertSorted(a, n);
  mean1 /= calls;
  printf("times for (first) parallel algorithm:\n");
  printf("\tbest (fastest) run: %f\n", best1);
  printf("\tworst (slowest) run: %f\n", worst1);
  printf("\tmean runtime: %f\n", mean1);


  if (compare != NULL) {
    printf("\n ===  ===  ===  ===  ===  ===  ===  ===  ===  ===  === \n\n");
    double times2[calls];
    double best2 = DBL_MAX;
    double worst2 = 0;
    double mean2 = 0;
    printf("sorting times with second parallel algorithm:\n");
    for(int i = 0; i < calls; i++) {
      generateArray(a, s, n, seed);
      start = mytime();
      compare(a, n, threads);
      stop = mytime();
      times2[i] = stop-start;
      if(times2[i] > worst2) worst2 = times2[i];
      if(times2[i] < best2) best2 = times2[i];
      mean2 += times2[i];
      //printf(" > %f\n", stop-start);
    }
    //assert only last solution
    assertSorted(a, n);
    mean2 /= calls;
    printf("times for second parallel algorithm:\n");
    printf("\tbest (fastest) run: %f\n", best2);
    printf("\tworst (slowest) run: %f\n", worst2);
    printf("\tmean runtime: %f\n", mean2);
  }

  printf("\n ===  ===  ===  ===  ===  ===  ===  ===  ===  ===  === \n\n");
  generateArray(a, s, n, seed);

  //call sequential algorithm
  printf("start sequential algorithm for comparison...\n");
  start = mytime();
  quicksortS(a, 0, n-1);
  stop = mytime();
  assertSorted(a, n);
  printf("time for sequential algorithm: %.5f\n", (stop-start));
  printf("\n");
  
  free(a);
  return 0;
}
