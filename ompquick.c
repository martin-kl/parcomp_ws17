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

void _partition(int a[], int start, int end, int pivotIndex, struct partitionResult * result) {
  int aa, i, j;
  i = start; j = end+1;

  printf("a vor partiton von %i nach %i\n", start, end);
  printArray(a+start, end-start+1);
  // move pivotIndex to beginning of array so we don't need to deal with running "over" the pivot
  if (start <= pivotIndex && pivotIndex <= end) {
    aa = a[pivotIndex]; a[pivotIndex] = a[0]; a[0] = aa;
    pivotIndex = 0;
  }
  for (;;) {
    while (++i<j&&a[i] < a[pivotIndex]); // has one advantage
    while (a[--j]>a[pivotIndex] && j>=start);
    if (i>=j) break;
    aa = a[i]; a[i] = a[j]; a[j] = aa;
  }

  // if pivot is between start and end
  if (start <= pivotIndex && pivotIndex <= end) {
    // swap pivot
    aa = a[pivotIndex]; a[pivotIndex] = a[j]; a[j] = aa;
    result->smaller = j-start;
    result->larger = ((end - start) + 1) - result->smaller - 1;
  } else {
    result->smaller = j-start+1;
    result->larger = ((end - start) + 1) - result->smaller;
  }
  printf("a nach partiton von %i nach %i\n", start, end);
  printArray(a+start, end-start+1);
}

void quicksort(int a[], int n, int maxThreads, int unit)
{
  printf("\n\n------cheffe thread---------\n");
  if (n <= unit) {
    printf("sequentially solving (n: %i)\n", n);
    printArray(a, n);
    printf("\n");
    seqQuickSort(a, 0, n-1);
    return;
  }
  int pivotIndex = randomNumberBetween(0, n-1);
  int pivotValue = a[pivotIndex];
  int smaller[maxThreads];
  int larger[maxThreads];
  int * helperArray = malloc(sizeof(int) * n);
  #pragma omp parallel
  {
    int threads = omp_get_num_threads();
    assert(maxThreads == threads);
    int nPThreads = n/threads;
    int i = omp_get_thread_num();
    int start, end;

    start = i*nPThreads;
    //if i is last thread and n%threads != 0 the end for the last thread is n
    if(n%threads != 0 && i == threads -1) {
      end = n-1;
    }else {
      end = (i+1) * nPThreads - 1;
    }
    #pragma omp critical
    {
      printf("\n\t\t------%i thread---------\n", i);
    //call _partition
    struct partitionResult result;
    _partition(a, start, end, pivotIndex, &result);
    memcpy(helperArray+start, a+start, (sizeof(int) * (end-start+1)));
    printf("helper: ");
    printArray(helperArray+start, end-start+1);
    printf("\n");
    smaller[i] = result.smaller;
    larger[i] = result.larger;
      printf("\t\t\tstart = %i, end = %i\n", start, end);
      printf("\t\t\tsmaller[i] = %i, larger[i] = %i\n", smaller[i], larger[i]);
      printf("\t\t\tpivotIndex: %i, pivotValue: %i\n", pivotIndex, pivotValue);
      printf("\t\t------end %i thread---------\n", i);
      if (start <= pivotIndex && pivotIndex <= end) {
        assert((smaller[i] + larger[i] + 1) == (end-start+1));
      } else {
        assert((smaller[i] + larger[i]) == (end-start+1));
      }
    }

#pragma omp barrier
#pragma omp single
    {
      printf("a before filling in smaller\n");
      printArray(a, n);
      for (int k = 1; k < threads; k++) {
        smaller[k] += smaller[k-1];
        larger[k] += larger[k-1];
      }
    }
    #pragma omp critical
    {
    // Write results to global array
    int smallerFromIndex = i == 0 ? 0 : smaller[i-1];
    int smallerToIndex = smaller[i];
    int helperI = start;
    for (int ai = smallerFromIndex; ai < smallerToIndex; ai++) {
      a[ai] = helperArray[helperI++];
    }
    printf("a after filling in smaller\n");
    printArray(a, n);

    if (start <= pivotIndex && pivotIndex <= end) {
      helperI++;
    }

    //offset 1 everywhere because of pivot
    int largerFromIndex = i == 0 ? smaller[threads-1]+1 : smaller[threads-1]+1+larger[i-1];
    int largerToIndex = smaller[threads-1]+1+larger[i];
    for (int ai = largerFromIndex; ai < largerToIndex; ai++) {
      a[ai] = helperArray[helperI++];
    }
    printf("a after filling in larger\n");
    printArray(a, n);
    }
  }
  a[smaller[maxThreads-1]] = pivotValue;
  printf("smaller[maxThreads-1]: %i\n", smaller[maxThreads-1]);
  printf("a after filling in pivot\n");
  printArray(a, n);
  printf("freeing (helperArray, n: %i,)\n", n);
  free(helperArray);
  printf("\n\n------end cheffe thread---------\n");
  quicksort(a, smaller[maxThreads-1], maxThreads, unit);
  quicksort(a+smaller[maxThreads-1]+1, larger[maxThreads-1], maxThreads, unit);
}

#define MICRO 1000000.0

int main(int argc, char *argv[])
{
  int i, n;
  int *a; int s; // sequence type 0, 1, ...

  int threads;

  unsigned seed;

  double start, stop;

  s = 0;
  n = 1;
  seed = 0;
  threads = 1;
  for (i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='t') i++,sscanf(argv[i],"%d",&threads); //number of threads
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed); //seed - currently NOT used! !!!! TODO
  }

  a = (int*)malloc(n*sizeof(int));

  if (threads > 0) {
    if (threads > omp_get_max_threads()) threads = omp_get_max_threads();
    omp_set_num_threads(threads);
  }

  printf("Number of threads %d, n=%d, seed=%d\n", threads, n, seed);

  if (s==0) {
    //for (i=0; i<n; i++) a[i] = (100-i)%27;
    a = generateIntPeriodicNumbers(n);
  } else if (s==1) {
    //for (i=0; i<n; i++) a[i] = 27; // worst-case input
    a = generateIntSameNumbers(n, 27);
  }else if (s==2) {
    a = generateIntAscendingNumbers(n);
  }else if (s==3) {
    a = generateIntDescendingNumbers(n);
  } else if (s == 4) {
    //srand(seed);
    //for (i=0; i<n; i++) a[i] = rand()%n;
    a = generateIntRandomNumbers(n, 0, 100);
  }else {
    printf("invalid input for type, only 0-4 is valid\n");
    return 0;
  }

  start = omp_get_wtime();
  quicksort(a,n, threads, 5);
  stop = omp_get_wtime();

  printArray(a, n);
  // verify
  for (i=0; i<n-1; i++) assert(a[i]<=a[i+1]);

  printf("Sorting time %.2f\n",(stop-start)*MICRO);

  free(a);
  return 0;
}
