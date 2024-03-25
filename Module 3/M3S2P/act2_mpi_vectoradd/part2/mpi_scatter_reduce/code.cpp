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

    // Allocate memory for storing the local sum
    int local_sum = 0;
    
    // Calculate the local sum of elements in local_v3
    for (int i = 0; i < local_size; i++) {
        local_sum += local_v3[i];
    }

    int total_sum; // Variable to store the total sum

    // Perform reduction operation to calculate the total sum
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Print total sum from root process
    if (rank == 0) {
        cout << "Total sum of all elements in v3: " << total_sum << endl;
    }

    delete[] local_v1;
    delete[] local_v2;
    delete[] local_v3;

    MPI_Finalize();
    return 0;
}

