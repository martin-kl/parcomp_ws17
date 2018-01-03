/* OpenMP task Quicksort */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "sqsort.h"

#include <omp.h>
#include "generator.h"

struct partitionResult {
  int smaller;
  int larger;
};

int randomNumberBetween(int low, int high) {
  srand((unsigned)time(NULL));
  double drandom = ((double)rand()) / ((double)RAND_MAX);
  return drandom * (high-low) + low;
}

void _partition(int a[], int start, int end, struct partitionResult * result, int pivotValue) {
  //precond: pivot is outside of range: start - end
  int aa, i, j;
  i = start-1; j = end+1;

  for (;;) {
    while (++i<j&&a[i] < pivotValue); // has one advantage
    while (a[--j] > pivotValue && j>=start);
    if (i>=j) break;
    aa = a[i]; a[i] = a[j]; a[j] = aa;
  }

  result->smaller = j-start+1;
  result->larger = ((end - start) + 1) - result->smaller;

}

void quicksort(int a[], int n, int maxThreads, int unit)
{
  if (n <= unit) {
    seqQuickSort(a, 0, n-1);
    return;
  }

  //start parallel quicksort
  int pivotIndex = randomNumberBetween(0, n-1);
  int pivotValue = a[pivotIndex];
  //switch pivot to first element
  a[pivotIndex] = a[0];
  a[0] = pivotValue;
  pivotIndex = 0;

  int smaller[maxThreads];
  int larger[maxThreads];
  int * helperArray = malloc(sizeof(int) * n);
  int actualThreads = maxThreads;
  #pragma omp parallel
  {
    int threads = omp_get_num_threads();
    actualThreads = threads;
    int nPThreads = n/threads;
    int i = omp_get_thread_num();
    int start, end;

    start = i*nPThreads;
    if(i == 0) start = 1;
    //if i is last thread and n%threads != 0 the end for the last thread is n
    end = i == threads-1 ? n-1 : (i+1) * nPThreads -1;

    //call _partition
    struct partitionResult result;
    _partition(a, start, end, &result, pivotValue);

    memcpy(helperArray+start, a+start, (sizeof(int) * (end-start+1)));

    smaller[i] = result.smaller;
    larger[i] = result.larger;

    assert((smaller[i] + larger[i]) == (end-start+1));

    #pragma omp barrier
    #pragma omp single
    {
      for (int k = 1; k < threads; k++) {
        smaller[k] += smaller[k-1];
        larger[k] += larger[k-1];
      }
    }
    // Write results to global array
    int smallerFromIndex = i == 0 ? 0 : smaller[i-1];
    int smallerToIndex = smaller[i];
    int helperI = start;
    for (int ai = smallerFromIndex; ai < smallerToIndex; ai++) {
      a[ai] = helperArray[helperI++];
    }

    //offset 1 everywhere because of pivot
    int largerFromIndex = i == 0 ? smaller[threads-1]+1 : smaller[threads-1]+1+larger[i-1];
    int largerToIndex = smaller[threads-1]+1+larger[i];
    for (int ai = largerFromIndex; ai < largerToIndex; ai++) {
      a[ai] = helperArray[helperI++];
    }
  }
  //pivotIndex should always be 0 here
  a[smaller[actualThreads-1]] = pivotValue;

  free(helperArray);
#pragma omp parallel for
  for(int k = 0; k < 2; k++) {
    if(k == 0) {
      quicksort(a, smaller[actualThreads-1], actualThreads, unit);
    }else {
      quicksort(a+smaller[actualThreads-1]+1, larger[actualThreads-1], actualThreads, unit);
    }
  }
}

#define MICRO 1000000.0

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


int main(int argc, char *argv[])
{
  int i, n;
  int *a; int s; // sequence type 0, 1, ...

  int threads;
  int CALLS = 1;

  unsigned seed = 0;

  double start, stop;

  s = 0;
  n = 1;
  seed = 0;
  threads = 1;
  for (i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='t') i++,sscanf(argv[i],"%d",&threads); //number of threads
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
  }

  a = (int*)malloc(n*sizeof(int));
  generateArray(a, s, n, seed);
  printf("start sequential algorithm for comparison...\n");
  double startSeq, endSeq;
  startSeq = omp_get_wtime();
  //call sequential algorithm
  seqQuickSort(a, 0, n-1);
  endSeq = omp_get_wtime();

  for (i=0; i<n-1; i++) assert(a[i]<=a[i+1]);

  printf("time for sequential algorithm: %.5f\n", (endSeq-startSeq));
  printf("\n");

  if (threads > 0) {
    if (threads > omp_get_max_threads()) threads = omp_get_max_threads();
    omp_set_num_threads(threads);
  }


  double durations[CALLS];
  for(int i = 0; i < CALLS; i++) {
    generateArray(a, s, n, seed);
    //call parallel algorithm
    start = omp_get_wtime();
    quicksort(a, n, threads, 1000);
    stop = omp_get_wtime();
    durations[i] = stop-start;
  }

  //pintArray(a, n);
  // verify
  for (i=0; i<n-1; i++) assert(a[i]<=a[i+1]);

  //printf("Sorting time %.2f\n",(stop-start));
  printf("sorting times with parallel algorithm using openmp:\n");
  for (int i = 0; i < CALLS; i++) {
    printf(" > %f\n", durations[i]);
  }
  //printf("Sorting time %.2f\n",(stop-start)*MICRO);

  free(a);
  return 0;
}
