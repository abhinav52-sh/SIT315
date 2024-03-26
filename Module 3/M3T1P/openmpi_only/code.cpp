/*
 * Name: Abhinav Sharma
 * Student ID: 2210994752
 * Unit: SIT315
 */

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

const int MATRIX_SIZE = 400;

int **matrix1, **matrix2, **matrix3;

// Function to initialize a matrix with random values if initialize is true
void init_matrix(int **&matrix, int rows, int cols, bool initialize) {
    matrix = new int*[rows];
    matrix[0] = new int[rows * cols];
    for (int i = 1; i < rows; ++i) {
        matrix[i] = matrix[i - 1] + cols;
    }

    if (initialize) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                matrix[i][j] = rand() % 100;
            }
        }
    }
}

// Function to perform matrix multiplication for a given range of rows
void multiply(int** mat1, int** mat2, int** mat3, int num_rows) {
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            mat3[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; ++k) {
                mat3[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

// Function to print a matrix
void print_matrix(int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << "--------------" << endl;
}

// Function executed by the head process
void head(int total_processes) {
    init_matrix(matrix1, MATRIX_SIZE, MATRIX_SIZE, true);
    init_matrix(matrix2, MATRIX_SIZE, MATRIX_SIZE, true);
    init_matrix(matrix3, MATRIX_SIZE, MATRIX_SIZE, false);

    int num_rows = MATRIX_SIZE / total_processes;
    int broadcast_size = (MATRIX_SIZE * MATRIX_SIZE);
    int scatter_gather_size = (MATRIX_SIZE * MATRIX_SIZE) / total_processes;

    auto start = high_resolution_clock::now();

    MPI_Scatter(&matrix1[0][0], scatter_gather_size, MPI_INT, &matrix1[0][0], 
                scatter_gather_size, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(&matrix2[0][0], broadcast_size, MPI_INT, 0, MPI_COMM_WORLD);

    multiply(matrix1, matrix2, matrix3, num_rows);

    MPI_Gather(MPI_IN_PLACE, scatter_gather_size, MPI_INT, &matrix3[0][0], 
                scatter_gather_size, MPI_INT, 0, MPI_COMM_WORLD);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    // print_matrix(matrix3, MATRIX_SIZE, MATRIX_SIZE);

    cout << "Time taken by function: " << duration.count() << " Microseconds" << endl;
}

// Function executed by worker processes
void node(int total_processes, int rank) {
    int num_rows = MATRIX_SIZE / total_processes;
    int broadcast_size = (MATRIX_SIZE * MATRIX_SIZE);
    int scatter_gather_size = (MATRIX_SIZE * MATRIX_SIZE) / total_processes;

    init_matrix(matrix1, MATRIX_SIZE, MATRIX_SIZE, true);
    init_matrix(matrix2, MATRIX_SIZE, MATRIX_SIZE, false);
    init_matrix(matrix3, MATRIX_SIZE, MATRIX_SIZE, false);

    MPI_Scatter(NULL, scatter_gather_size, MPI_INT, &matrix1[0][0], 
                scatter_gather_size, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(&matrix2[0][0], broadcast_size, MPI_INT, 0, MPI_COMM_WORLD);

    multiply(matrix1, matrix2, matrix3, num_rows);

    MPI_Gather(&matrix3[0][0], scatter_gather_size, MPI_INT, NULL, 
                scatter_gather_size, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
    srand(time(0));

    int total_processes;
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
        head(total_processes);
    else
        node(total_processes, rank);

    MPI_Finalize();

    return 0;
}
