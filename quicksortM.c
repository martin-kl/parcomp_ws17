/* TUW, October 2011 */

#include <stdio.h>
#include <stdlib.h>

// MPI header
#include <mpi.h>

#define HELLO 1234 // tag for control messages

int main(int argc, char *argv[])
{
  int rank, size;
  int prev;
  char name[MPI_MAX_PROCESSOR_NAME];
  int nlen;


  MPI_Init(&argc,&argv);

  int s = 0;
  int n = 1;
  unsigned seed = 0;

  for (int i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
  }

  generateArray(a, s, n, seed);
  start = mytime();
  implementation(a, n, threads);
  stop = mytime();
  assertSorted(a, n);
  printf(" > %f\n", stop-start);

  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  MPI_Get_processor_name(name,&nlen);

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
