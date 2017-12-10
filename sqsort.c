//serial qsort implementation for reference:
//from: geeksforgeeks.org
#include <stdio.h>
#include "generator.h"

/* The main function that implements QuickSort
	arr[] --> Array to be sorted,
	low  --> Starting index,
	high  --> Ending index */
void quickSort(int arr[], int low, int high);
 
/* This function takes last element as pivot, places
	the pivot element at its correct position in sorted
	array, and places all smaller (smaller than pivot)
	to left of pivot and all greater elements to right
	of pivot */

int partition (int arr[], int low, int high);
// A utility function to swap two elements
void swap(int* a, int* b);
/* Function to print an array */
void printArray(int arr[], int size);


//### Implementation ###//


int main() {
    /*int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    quickSort(arr, 0, n-1);
    printf("Sorted array: ");
    printArray(arr, n);
    return 0;
    */
    int* array = generateIntAscendingNumbers(ARRAY_SIZE);
    printf("generated array (int ascending): ");
    printArray(array, ARRAY_SIZE);
    quickSort(array, 0, ARRAY_SIZE);
    printf("sorted array: ");
    printArray(array, ARRAY_SIZE);

    int* array2 = generateIntDescendingNumbers(ARRAY_SIZE);
    printf("generated array (int descending): ");
    printArray(array2, ARRAY_SIZE);
    quickSort(array2, 0, ARRAY_SIZE);
    printf("sorted array: ");
    printArray(array2, ARRAY_SIZE);

    int* array3 = generateIntSameNumbers(ARRAY_SIZE);
    printf("generated array (int same numbers): ");
    printArray(array3, ARRAY_SIZE);
    quickSort(array3, 0, ARRAY_SIZE);
    printf("sorted array: ");
    printArray(array3, ARRAY_SIZE);

    int* array4 = generateIntRandomNumbers(ARRAY_SIZE, RANDOM_INT_MIN, RANDOM_INT_MAX);
    printf("generated array (int random): ");
    printArray(array4, ARRAY_SIZE);
    quickSort(array4, 0, ARRAY_SIZE);
    printf("sorted array: ");
    printArray(array4, ARRAY_SIZE);

    return 0;
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int partition (int arr[], int low, int high) {
    int pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element
 
    for (int j = low; j <= high- 1; j++) {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot) {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
 
void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}
  
void printArray(int arr[], int size) {
    printf("[");
    int i;
    for (i=0; i < size-1; i++)
        printf("%d ", arr[i]);
    printf("%d]\n", arr[size-1]);
}
