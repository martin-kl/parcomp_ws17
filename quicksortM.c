/* TUW, October 2011 */

#include <stdio.h>
#include <stdlib.h>

// MPI header
#include <mpi.h>

void assertSorted(int * a, int n) {
  for (int i=0; i<n-1; i++) assert(a[i]<=a[i+1]);
}


int main(int argc, char *argv[])
{
  int rank, size;
  int prev;
  char name[MPI_MAX_PROCESSOR_NAME];
  int nlen;
  int * a;


  MPI_Init(&argc,&argv);

  //variables for quicksort:
  double start, stop;
  int s = 0;
  int n = 1;
  unsigned seed = 0;

  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  MPI_Get_processor_name(name,&nlen);

  //now every processor does this - probably not that bad... (but not really needed)
  for (int i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
  }

  if(rank == 0) {
    //malloc array - is this working if every processor has a? (defined after main header)
    a = (int*)malloc(n*sizeof(int));
    printf("Executing with: -n %i -s %i -S %i\n", n, s, seed);
    generateArray(a, s, n, seed);
    start = MPI_Wtime();
  }
  //TODO start quicksort
  //first step: scatter data, or even generate data scattered?





  if(rank == 0) {
    stop = MPI_Wtime();
    assertSorted(a, n);
    printf(" > %f\n", stop-start);
  }

  //not from us:
  if (rank==0) {
    printf("Rank %d initializing, total %d\n",rank,size);
  } else {
    MPI_Recv(&prev,1,MPI_INT,rank-1,HELLO,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Rank %d on %s received from %d, passing on\n",rank,name,prev);
  }
  if (rank+1<size) MPI_Send(&rank,1,MPI_INT,rank+1,HELLO,MPI_COMM_WORLD);


  MPI_Finalize();
  return 0;
}
