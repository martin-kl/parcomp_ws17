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



// --- --- --- --- ---- --- --- ---
// Method Implementation
// --- --- --- --- ---- --- --- ---


void quicksortO(int a[], int n, int maxThreads)
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

  free(helperArray);
#pragma omp parallel for
  for(int k = 0; k < 2; k++) {
    if(k == 0) {
      quicksortO(a, smaller[actualThreads-1], actualThreads);
    }else {
      quicksortO(a+smaller[actualThreads-1]+1, larger[actualThreads-1], actualThreads);
    }
  }
}
