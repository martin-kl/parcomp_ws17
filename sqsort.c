//serial qsort implementation for reference:
//from: geeksforgeeks.org

#include "sqsort.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "sqsort.h"

#include <omp.h>
#include "generator.h"

/* This function takes last element as pivot, places
	the pivot element at its correct position in sorted
	array, and places all smaller (smaller than pivot)
	to left of pivot and all greater elements to right of pivot */
int partition (int a[], int start, int end, int pivotValue);
// A utility function to swap two elements
void swap(int* a, int* b);

int randomNumberBetween2(int low, int high) {
  srand((unsigned)time(NULL));
  double drandom = ((double)rand()) / ((double)RAND_MAX);
  return drandom * (high-low) + low;
}



void seqQuickSort(int arr[], int low, int high) {
  if (low < high) {
    /* pi is partitioning index, arr[p] is now
       at right place */

    int pivotIndex = randomNumberBetween2(low, high);
    int pivotValue = arr[pivotIndex];
    //switch pivot to first element
    arr[pivotIndex] = arr[0];
    arr[0] = pivotValue;


    int pi = partition(arr, low+1, high, pivotValue);

    // Separately sort elements before
    // partition and after partition
    seqQuickSort(arr, low, pi - 1);
    seqQuickSort(arr, pi + 1, high);
  }
}

int partition (int a[], int start, int end, int pivotValue) {
  int aa, i, j;
  i = start-1; j = end+1;

  for (;;) {
    while (++i<j&&a[i] < pivotValue); // has one advantage
    while (a[--j] > pivotValue && j>=start);
    if (i>=j) break;
    aa = a[i]; a[i] = a[j]; a[j] = aa;
  }
  aa = a[0]; a[0] = a[j]; a[j] = aa;
  return j;
}

void swap(int* a, int* b) {
  int t = *a;
  *a = *b;
  *b = t;
}
