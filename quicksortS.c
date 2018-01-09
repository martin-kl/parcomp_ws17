//serial qsort implementation for reference:
//partly from: geeksforgeeks.org

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "sorts.h"
#include "generator.h"
#include "shared.h"

// --- --- --- --- --- --- --- --- ---
// Method Implementation
// --- --- --- --- --- --- --- --- ---

void quicksortS(int arr[], int low, int high) {
  if (low < high) {
    /* pi is partitioning index, arr[p] is now at right place */

    int pivotIndex = randomNumberBetween(low, high);
    int pivotValue = arr[pivotIndex];
    //switch pivot to first element
    arr[pivotIndex] = arr[low];
    arr[low] = pivotValue;

    struct partitionResult result;
    partition(arr, low+1, high, &result, pivotValue);
    int pi = low+result.smaller;
    int aa = arr[low]; arr[low] = arr[pi]; arr[pi] = aa;

    // Separately sort elements before
    // partition and after partition
    quicksortS(arr, low, pi - 1);
    quicksortS(arr, pi + 1, high);
  }
}
