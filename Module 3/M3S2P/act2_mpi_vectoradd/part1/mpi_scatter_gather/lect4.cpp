/*
 * Name: Abhinav Sharma
 * Student ID: 2210994752
 * Unit: SIT315
 */

#include <mpi.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <time.h>

using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 100;
    }
}

int main(int argc, char* argv[]) {
    unsigned long size = 1000000;
    int rank, num_procs;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    srand(time(0) + rank);  // Seed random number generator uniquely for each process

    int local_size = size / num_procs; // Calculate local chunk size per process
    int* local_v1 = new int[local_size];
    int* local_v2 = new int[local_size];
    int* local_v3 = new int[local_size];

    if (rank == 0) {
        // Root process generates and scatters data
        int* v1 = new int[size];
        int* v2 = new int[size];
        randomVector(v1, size);
        randomVector(v2, size);

        MPI_Scatter(v1, local_size, MPI_INT, local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(v2, local_size, MPI_INT, local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);

        delete[] v1;
        delete[] v2;
    } else {
        // Receive scattered data on other processes
        MPI_Scatter(MPI_BOTTOM, 0, MPI_INT, local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(MPI_BOTTOM, 0, MPI_INT, local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Perform local vector addition
    for (int i = 0; i < local_size; i++) {
        local_v3[i] = local_v1[i] + local_v2[i];
    }

    auto start = high_resolution_clock::now();

    // Gather local results back to the root process
    if (rank == 0) {
        int* v3 = new int[size];
        MPI_Gather(local_v3, local_size, MPI_INT, v3, local_size, MPI_INT, 0, MPI_COMM_WORLD);

        // No need to calculate local sum here as it's already gathered
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by MPI program with Scatter and Gather: " << duration.count() 
             << " microseconds" << endl;

        delete[] v3;
    } else {
        MPI_Gather(local_v3, local_size, MPI_INT, MPI_BOTTOM, 0, MPI_INT, 0, MPI_COMM_WORLD);
    }

    delete[] local_v1;
    delete[] local_v2;
    delete[] local_v3;

    MPI_Finalize();
    return 0;
}
