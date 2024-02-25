/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Task: M2S3P
* Unit: SIT315
*/

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <time.h>

using namespace std::chrono;
using namespace std;

// global variables
const unsigned long vectorSize = 100000000;

// Function to fill the vector with random values
void fillVector(int vector[], int size) {
  for (int i = 0; i < size; i++) {
    vector[i] = 2;
  }
}

int main() {
  // Seed for random number generation
  srand(time(0));

  // Declaring vectors
  int *v1, *v2, *v3;

  // Assigning memory to each vector pointer
  v1 = (int *)malloc(vectorSize * sizeof(int));
  v2 = (int *)malloc(vectorSize * sizeof(int));
  v3 = (int *)malloc(vectorSize * sizeof(int));

  // Filling vectors v1 and v2 with random values
  fillVector(v1, vectorSize);
  fillVector(v2, vectorSize);

  // Recording start time 
  double startTime = omp_get_wtime();

  // Performing vector addition in parallel
  int total = 0;
  #pragma omp parallel for shared(v1, v2, v3) reduction(+:total)
  for (int i = 0; i < vectorSize; i++) {
    v3[i] = v1[i] + v2[i];
    total += v3[i];
  }

  // Recording stop time 
  double stopTime = omp_get_wtime();

  // Calculating and printing the duration time
  double executionTime = stopTime - startTime;
  cout << "Time taken by function: " << executionTime * 1e6 << " microseconds" << endl;

  // Printing the total sum
  cout << "Total sum using reduction: " << total << endl;

  // Freeing allocated memory
  free(v1);
  free(v2);
  free(v3);

  return 0;
}
