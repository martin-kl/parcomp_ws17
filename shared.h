int randomNumberBetween(int low, int high);
struct partitionResult {
  int smaller;
  int larger;
};
void partition(int a[], int start, int end, struct partitionResult * result, int pivotValue);

double mytime();
