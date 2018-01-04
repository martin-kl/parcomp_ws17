#include "shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int initializedGenerator = 0;
int randomNumberBetween(int low, int high) {
  if (initializedGenerator == 0) {
    srand((unsigned)mytime());
    initializedGenerator = 1;
  }
  double drandom = ((double)rand()) / ((double)RAND_MAX);
  return drandom * (high-low) + low;
}

void printArray(int arr[], int size) {
  printf("[");
  int i;
  for (i=0; i < size-1; i++)
    printf("%d ", arr[i]);
  printf("%d]\n", arr[size-1]);
}

/*
 * places all smaller than pivotValue starting from start index and all greater elements to right of largest smaller than pivotValue
 * the pivot element is outside of range: start - end
 * it writes into partitionResult how many are smaller than the pivotValue and how many are larger than the pivotValue
 */
void partition(int a[], int start, int end, struct partitionResult * result, int pivotValue) {
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


typedef unsigned long long usecs;

double mytime() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return ((double)((usecs)now.tv_usec + (usecs)now.tv_sec*1000000.0))/1000000.0;
}
