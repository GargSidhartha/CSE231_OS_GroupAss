#include "simple-multithreader.h"
#include <assert.h>
#include <time.h>

clock_t start,end;

int main(int argc, char** argv) {
  // intialize problem size
  int numThread = argc>1 ? atoi(argv[1]) : 2;
  int size = argc>2 ? atoi(argv[2]) : 1024;  
  // allocate matrices
  int** A = new int*[size];
  int** B = new int*[size];
  int** C = new int*[size];
  start = clock();
  parallel_for(0, size, [=](int i) {
    A[i] = new int[size];
    B[i] = new int[size];
    C[i] = new int[size];
    for(int j=0; j<size; j++) {
      // initialize the matrices
      std::fill(A[i], A[i]+size, 1);
      std::fill(B[i], B[i]+size, 1);
      std::fill(C[i], C[i]+size, 0);
    }
  }, numThread);  
  end = clock();
  printf("The exec time is : %f\n",((double)(end-start))/CLOCKS_PER_SEC);
  // start the parallel multiplication of two matrices
  start = clock();
  parallel_for(0, size, 0, size, [&](int i, int j) {
    for(int k=0; k<size; k++) {
      C[i][j] += A[i][k] * B[k][j];
    }
  }, numThread);
  end = clock();
  printf("The exec time is : %f\n",((double)(end-start))/CLOCKS_PER_SEC);
  // verify the result matrix
  for(int i=0; i<size; i++) for(int j=0; j<size; j++) assert(C[i][j] == size);
  printf("Test Success. \n");
  // cleanup memory
  start = clock();
  parallel_for(0, size, [=](int i) {
    delete [] A[i];
    delete [] B[i];
    delete [] C[i];
  }, numThread);
  end = clock();
  printf("The exec time is : %f\n",((double)(end-start))/CLOCKS_PER_SEC);
  delete[] A;
  delete[] B;
  delete[] C;
  return 0;
}
