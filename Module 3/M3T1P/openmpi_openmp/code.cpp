#include <iostream>
#include <omp.h>
#include <mpi.h>
#include <iomanip>

#define N 400

void matrixMultiply(int *a, int *b, int *c, int rows, int cols, int shared_dim, int rank, int size) {
    // matrix multiplication logic with OpenMP parallelization
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int sum = 0;
            #pragma omp parallel for reduction(+:sum)
            for (int k = 0; k < shared_dim; ++k) {
                sum += a[i * shared_dim + k] * b[k * cols + j];
            }
            c[i * cols + j] = sum;
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Initializing matrices A, B, C
    int *A = new int[N * N];
    int *B = new int[N * N];
    int *C = new int[N * N];

    // Measuring start time
    double startTime = MPI_Wtime();

    // Dividing work among MPI processes
    int rows_per_process = N / size;
    int *localA = new int[rows_per_process * N];
    int *localC = new int[rows_per_process * N];

    MPI_Scatter(A, rows_per_process * N, MPI_INT, localA, rows_per_process * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcasting matrix B to all processes
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Performing local matrix multiplication with OpenMP
    matrixMultiply(localA, B, localC, rows_per_process, N, N, rank, size);

    // Gathering results back to the root process
    MPI_Gather(localC, rows_per_process * N, MPI_INT, C, rows_per_process * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Measuring end time
    double endTime = MPI_Wtime();

    // Cleaning up and finalizing MPI
    delete[] A;
    delete[] B;
    delete[] C;
    delete[] localA;
    delete[] localC;
    MPI_Finalize();

    // Output execution time
    if (rank == 0) {
        std::cout << "Execution time: " << std::fixed << std::setprecision(6) << endTime - startTime 
                  << " seconds" << std::endl;
    }

    return 0;
}
