/* OpenMP task Quicksort */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <omp.h>

#include "sorts.h"
#include "generator.h"
#include "shared.h"

#define UNIT (1000)


void quicksortOImpl(int a[], int n, int maxThreads);

// --- --- --- --- ---- --- --- ---
// Method Implementation
// --- --- --- --- ---- --- --- ---
//
void quicksortOImpl2(int a[], int n, int maxThreads, int * helperArray);

void quicksortO(int a[], int n, int threads) {
  if (threads > 0) {
    if (threads > omp_get_max_threads()) threads = omp_get_max_threads();
    omp_set_num_threads(threads);
  }
  //int * helperArray = malloc(sizeof(int) * n);
  //quicksortOImpl2(a, n, threads, helperArray);
  //free(helperArray);
  quicksortOImpl(a, n, threads);
}

void quicksortOImpl(int a[], int n, int maxThreads) {
  if (n <= UNIT) {
    quicksortS(a, 0, n-1);
    return;
  }
  int low = 0;
  int high = n-1;
  if (low < high) {
    /* pi is partitioning index, arr[p] is now
       at right place */

    int pivotIndex = randomNumberBetween(low, high);
    int pivotValue = a[pivotIndex];
    //switch pivot to first element
    a[pivotIndex] = a[low];
    a[low] = pivotValue;


    struct partitionResult result;
    partition(a, low+1, high, &result, pivotValue);
    int pi = low+result.smaller;
    int aa = a[low]; a[low] = a[pi]; a[pi] = aa;

    #pragma omp parallel
    #pragma omp for
    for(int k = 0; k < 2; k++) {
    //printf("threads: %i\n", omp_get_num_threads());
      if(k == 0) {
        //printf("starting first %i\n", n);
        quicksortOImpl(a, pi, maxThreads);
        //printf("ending first %i\n", n);
      }else {
         //printf("starting second %i\n", n);
        quicksortOImpl(a+pi+1, n-pi-1, maxThreads);
         //printf("ending second %i\n", n);
      }
    }
  }
}

void quicksortOImpl2(int a[], int n, int maxThreads, int * helperArray)
{
  if (n <= UNIT) {
    quicksortS(a, 0, n-1);
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

    struct partitionResult result;
    partition(a, start, end, &result, pivotValue);

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


#pragma omp parallel for
  for(int k = 0; k < 2; k++) {
    if(k == 0) {
      quicksortOImpl2(a, smaller[actualThreads-1], actualThreads, helperArray);
    }else {
      quicksortOImpl2(a+smaller[actualThreads-1]+1, larger[actualThreads-1], actualThreads, helperArray+smaller[actualThreads-1]+1);
    }
  }
}
