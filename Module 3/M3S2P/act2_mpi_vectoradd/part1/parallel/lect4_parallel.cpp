/*
 * Name: Abhinav Sharma
 * Student ID: 2210994752
 * Unit: SIT315
 */

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <time.h>

using namespace std::chrono;
using namespace std;

// Define the number of threads
#define NUM_THREADS 4

// Define a struct to pass arguments to the thread function
struct ThreadData {
    int* v1;
    int* v2;
    int* v3;
    int start;
    int end;
};

// Function for generating a random vector of integers
void randomVector(int vector[], int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 100; // Generating random integers between 0 and 99 by the use of rand() function.
    }
}

// Function for parallel addition of vector elements
void* addVectors(void* arg) {
    ThreadData* data = (ThreadData*)arg;  
    for (int i = data->start; i < data->end; i++) {
        data->v3[i] = data->v1[i] + data->v2[i];
    }
    pthread_exit(NULL);
}

int main() {
    // Size of the vector
    unsigned long size = 1000000; 
    srand(time(0));                 // Seed the random number generator

    int* v1 = (int*)malloc(size * sizeof(int));
    int* v2 = (int*)malloc(size * sizeof(int));
    int* v3 = (int*)malloc(size * sizeof(int));

    // Filling the arrays with random integers
    randomVector(v1, size);
    randomVector(v2, size);

    for (int partitionFactor = 1; partitionFactor <= 8; partitionFactor++) {
    auto start = high_resolution_clock::now(); // Start time measurement

    // Create threads for parallel addition
    pthread_t threads[partitionFactor];
    ThreadData threadData[partitionFactor];


    int chunk_size = size / partitionFactor;
    for (int i = 0; i < partitionFactor; i++) {
        threadData[i].v1 = v1;
        threadData[i].v2 = v2;
        threadData[i].v3 = v3;
        threadData[i].start = i * chunk_size;
        threadData[i].end = (i == partitionFactor - 1) ? size : (i + 1) * chunk_size;

        int rc = pthread_create(&threads[i], NULL, addVectors, (void*)&threadData[i]);
    }

    // Join the threads
    for (int i = 0; i < partitionFactor; i++) {
        pthread_join(threads[i], NULL);
    }

    auto stop = high_resolution_clock::now(); // Stop time measurement
    auto duration = duration_cast<microseconds>(stop - start); // Calculate duration

    std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
    }
    // Freeing the dynamically allocated memory
    free(v1);
    free(v2);
    free(v3);

    return 0;
}

