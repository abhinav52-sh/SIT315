#include <chrono>
#include <cstdlib>
#include <iostream>
#include <time.h>

using namespace std::chrono;
using namespace std;

// Function to fill the vector with random values between 0 and 99
void randomVector(int vector[], int size) {
  for (int i = 0; i < size; i++) {
    vector[i] = rand() % 100;
  }
}

int main() {
  // Setting the size of vectors
  unsigned long size = 1000000000;

  // Seed for random number generation
  srand(time(0));

  // Declaring vectors pointers
  int *v1, *v2, *v3;

  // Allocating memory for vector pointers v1, v2, and v3 to point to
  v1 = (int *)malloc(size * sizeof(int));
  v2 = (int *)malloc(size * sizeof(int));
  v3 = (int *)malloc(size * sizeof(int));

  // Recording start time
  auto start = high_resolution_clock::now();  
  
  // Filling vectors v1 and v2 with random values
  randomVector(v1, size);
  randomVector(v2, size);

  // Performing vector addition
  for (int i = 0; i < size; i++) {
    v3[i] = v1[i] + v2[i];
  }

  // Recording stop time
  auto stop = high_resolution_clock::now();

  // Calculating and printing the duration time to perform the vector addition
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Time taken by function: " << duration.count() << " microseconds"<< endl;

  // Freeing the memory allocated to vector pointers
  free(v1);
  free(v2);
  free(v3);

  return 0;
}

