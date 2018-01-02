/* OpenMP task Quicksort */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <omp.h>


int randomNumberBetween(int low, int high) {
  srand((unsigned)time(NULL));
  double drandom = rand() / RAND_MAX;
  return drandom * (high-low) + low;
}

int partition(int a[], int start, int end, int pivotIndex) {
  int aa, i, j;
  i = start; j = end;
  for (;;) {
    while (++i<j&&a[i] < a[pivotIndex]); // has one advantage
    while (a[--j]>a[pivotIndex]);
    if (i>=j) break;
    aa = a[i]; a[i] = a[j]; a[j] = aa;
  }
  // swap pivot
  aa = a[pivotIndex]; a[pivotIndex] = a[j]; a[j] = aa;
  return j-start-1;
}

void quicksort(int a[], int n)
{
  int pivotIndex = randomNumberBetween(0, n-1);
#pragma omp parallel
  {
  int threads = omp_get_num_threads();
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

  //call partition
  int smaller = partition(a, start, end, pivotIndex);
  int larger = end - start - smaller - 1;
  assert((smaller + larger + 1) == (start + end));

#pragma omp barrier

  }
}

#define MICRO 1000000.0

int main(int argc, char *argv[])
{
  int i, n;
  int *a;
  int s; // sequence type 0, 1, ...

  int threads;

  unsigned seed;

  double start, stop;

  s = 0;
  n = 1;
  seed = 0;
  threads = 0;
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

  printf("Number of threads %d, n=%d, seed=%d\n",omp_get_num_threads(), n, seed);

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
#pragma omp parallel
  {
#pragma omp single nowait
    quicksort(a,n);
#pragma omp taskwait
  }
  stop = omp_get_wtime();

  // verify
  for (i=0; i<n-1; i++) assert(a[i]<=a[i+1]);

  printf("Sorting time %.2f\n",(stop-start)*MICRO);

  free(a);
  return 0;
}
