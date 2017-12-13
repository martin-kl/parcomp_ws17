#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> //for memcpy
#include <sys/time.h>

#include "generator.h"
#include "sqsort.h"
#include "cqsort.h"
#include "timeFunction.h"

//### Implementation ###//


int main(int argc, char *argv[]) {
  //array for sequential Sort
  int *array;
  //array for sequential Sort
  int *array2;

  int type = 0;
  int datatype = 0;
  unsigned seed;
  int n = ARRAY_SIZE;

  usecs startSeq, stopSeq;
  usecs startPar, stopPar;

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
    array = generateIntPeriodicNumbers(n);
  }else if(type == 1) {
   array = generateIntSameNumbers(n, 27);
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

  //copy array so we can sort it once sequential and once parallel
  array2 = malloc(n * sizeof(int));
  memcpy(array2, array, n * sizeof(int));
  //this check memcpy works - the check below found no error
  /*
  for(int i = 0; i < n; i++) {
    if(array[i] != array2[i]) {
      printf("found error on i=%i, a[i]=%i, a2[i]=%i\n", i, array[i], array2[i]);
    }
  }
  */

  /*
  printf("generated array: ");
  printArray(array, n);
  */
  startSeq = mytime();
  seqQuickSort(array, 0, n-1);
  stopSeq = mytime();
  /*
  printf("sorted array: ");
  printArray(array, n);
  */
    
  //verify result:
  for (int i = 0; i < n-1; i++) assert(array[i] <= array[i+1]);

  printf("Sorting time for sequential algorithm: %lld\n", stopSeq-startSeq);

  //
  //now start parallel algorithm and meassure time too
  //
 
  /*
  printf("generated array: ");
  printArray(array, n);
  */
  startPar = mytime();
  //with unit size = 1000
  cilkQuickSort(array2, n, 1000);
  stopPar = mytime();
  /*
  printf("sorted array: ");
  printArray(array, n);
  */
      
  //verify result:
  for (int i = 0; i < n-1; i++) assert(array2[i] <= array2[i+1]);

  printf("Sorting time for parallel algorithm: %lld\n", stopPar-startPar);

  free(array);
  free(array2);
  return 0;
}
