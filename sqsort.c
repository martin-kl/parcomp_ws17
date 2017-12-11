//serial qsort implementation for reference:
//from: geeksforgeeks.org
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
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

//quick hack for time measurement:
typedef unsigned long long usecs;

usecs mytime() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (usecs)now.tv_usec + (usecs)now.tv_sec*1000000L;
}

//### Implementation ###//


int main(int argc, char *argv[]) {
    int *array;
    int type = 0;
    int datatype = 0;
    unsigned seed;
    int n = ARRAY_SIZE;

    usecs start, stop;

    for(int i = 1; i < argc && argv[i][0] =='-'; i++) {
        if (argv[i][1] == 'n') {
            i++;
            sscanf(argv[i], "%d", &n); //number of elements
            assert(n > 0);
        }else if (argv[i][1] == 't') {
            i++;
            sscanf(argv[i], "%d", &type); //sequence type, 0=irgendwas, 1=27 on every number, 2=ascending, 3=descending, 4=random
            //FIXME real value for type < xx
            assert(type >= 0 && type < 5);
        }else if (argv[i][1] == 's') {
            //TODO seed is not used yet - should we pass this to the generator?
            i++;
            sscanf(argv[i], "%d", &seed); //seed
        }else if(argv[i][1] == 'd') {
            //STILL TODO - cause array of values is only int or should we add both (int and double array??)
            i++;
            sscanf(argv[i], "%d", &datatype); //datatype, 0 == int, 1 == double
            assert((datatype == 0) || (datatype == 1));
        }else {
            printf("invalid argument, valid syntax: ./sqsort [-n xxxx] [-d x] [-t x] [-s xxx]\n");
            return(0);
        }
    }

    //generate array for values:
    //TODO fix this for all datatypes
    if(type == 0) {
        //this case was already in the presets from the homepage, creates a descending array
        array = (int *)malloc(sizeof(int) * n);
        for(int i = 0; i < n; i++) array[i] = (100-i)%27;
    }else if(type == 1) {
        array = generateIntSameNumbers(n);
    }else if(type == 2) {
        array = generateIntAscendingNumbers(n);
    }else if(type == 3) {
        array = generateIntDescendingNumbers(n);
    }else if(type == 4) {
        array = generateIntRandomNumbers(n, 0, 100);
    }else {
        printf("invalid type range, 0-4 is only valid\n");
        return 0;
    }

    /*
    printf("generated array: ");
    printArray(array, n);
    */

    start = mytime();
    quickSort(array, 0, n);
    stop = mytime();

    /*
    printf("sorted array: ");
    printArray(array, n);
    */
    
    //verify result:
    for (int i = 0; i < n-1; i++) assert(array[i] <= array[i+1]);

    printf("Sorting time %lld\n", stop-start);

    free(array);
    return 0;
}

void testEverythingWithInts() {
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
