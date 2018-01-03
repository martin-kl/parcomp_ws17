#include "shared.h"
#include <stdlib.h>
#include <time.h>

int randomNumberBetween(int low, int high) {
  srand((unsigned)time(NULL));
  double drandom = ((double)rand()) / ((double)RAND_MAX);
  return drandom * (high-low) + low;
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

