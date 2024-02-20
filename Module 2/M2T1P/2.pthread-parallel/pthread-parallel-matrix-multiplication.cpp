#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>
#include <fstream>
#include <pthread.h>

using namespace std::chrono;
using namespace std;

const int matrixSize = 1000;
#define MAX_THREADS 4

// Global Variables
int matrixA[matrixSize][matrixSize];
int matrixB[matrixSize][matrixSize];
int matrixC[matrixSize][matrixSize];
int threadNum = 0;
int threadSize = matrixSize / MAX_THREADS;

// filliing a Matrix with random values between 1-9
void fillMatrix(int matrix[][matrixSize])
{
    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            matrix[i][j] = rand() % 10;
        }
    }
}

void *multiplyMatrix_pthread(void *args)
{
    // Current thread and the partition size
    int thread = threadNum++;

    for (int i = thread * threadSize; i < (thread + 1) * threadSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            matrixC[i][j] = 0;

            for (int k = 0; k < matrixSize; ++k)
            {
                matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
    return NULL;
}

// save matrix elements value to a file
void saveMatrixFile(int matrix[][matrixSize])
{
    string fileName = "pthread-matrix-multiplication.txt";
    ofstream file(fileName);
    if(file.is_open()) {
        for(int i = 0; i < matrixSize; i++)
        {
            for(int j = 0; j < matrixSize; j++)
            {
                file << matrix[i][j] << "   ";
            }
            file << "\n\n";
        }
        file.close();
        cout << "Output Matrix written to " << fileName << endl;
    }
    else
    {
        cout << "Failed to open the file: " << fileName << endl;
    }
}

int main()
{
    // To randomize the rand() function
    srand(time(0));

    // Filling the matrices
    fillMatrix(matrixA);
    fillMatrix(matrixB);

    // Starts the timer
    auto start = high_resolution_clock::now();

    // Initializing threads
    pthread_t matrix_thread[MAX_THREADS];

    // Creating threads
    for (int i = 0; i < MAX_THREADS; ++i)
    {
        pthread_create(&matrix_thread[i], NULL, &multiplyMatrix_pthread, NULL);
    }

    // Joining threads
    for (int i = 0; i < MAX_THREADS; ++i)
    {
        pthread_join(matrix_thread[i], NULL);
    }

    // Get the current time when its stopped
    auto stop = high_resolution_clock::now();

    // Returns the time it takes for the function to run
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken for pthread matrix multiplication: " << duration.count() << " microseconds"<< endl;

    saveMatrixFile(matrixC);

    return 0;
}