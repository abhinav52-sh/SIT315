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

const unsigned long vectorSize = 100000000;

void fillVector(int vector[], int size) {
  for (int i = 0; i < size; i++) {
    vector[i] = rand() % 100;
  }
}

int main() {
  srand(time(0));

  int *v1, *v2, *v3;
  v1 = (int *)malloc(vectorSize * sizeof(int));
  v2 = (int *)malloc(vectorSize * sizeof(int));
  v3 = (int *)malloc(vectorSize * sizeof(int));

  fillVector(v1, vectorSize);
  fillVector(v2, vectorSize);

  double startTime, stopTime, executionTime;

  // Experimenting with different OpenMP scheduling types and chunk sizes
  for (int scheduleType = 1; scheduleType <= 3; scheduleType++) {
    for (int chunkSize = 1; chunkSize <= 8; chunkSize *= 2) {
      startTime = omp_get_wtime();

      #pragma omp parallel for shared(v1, v2, v3) schedule(static, chunkSize)
      for (int i = 0; i < vectorSize; i++) {
        v3[i] = v1[i] + v2[i];
      }

      stopTime = omp_get_wtime();

      executionTime = stopTime - startTime;
      cout << "Schedule type: " << scheduleType << ", Chunk size: " << chunkSize
           << ", Time: " << executionTime * 1e6 << " microseconds" << endl;
    }
  }

  free(v1);
  free(v2);
  free(v3);

  return 0;
}
