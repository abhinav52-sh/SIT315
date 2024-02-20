#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <fstream>
#include <omp.h>

using namespace std::chrono;
using namespace std;

const int matrixSize = 800;
#define MAX_THREADS 3

// Global Variables
int matrixA[matrixSize][matrixSize];
int matrixB[matrixSize][matrixSize];
int matrixC[matrixSize][matrixSize];

// filliing a Matrix with random values between 1-9
void fillMatrix(int matrix[][matrixSize])
{
    for (int i = 0; i < matrixSize; ++i)
    {
        for (int j = 0; j < matrixSize; ++j)
        {
            matrix[i][j] = 1;
        }
    }
}

void multiplyMatrix_openmp()
{
    #pragma omp parallel for collapse(2) num_threads(MAX_THREADS)
    for (int i = 0; i < matrixSize; ++i)
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
}

void saveMatrixFile(int matrix[][matrixSize])
{
    string fileName = "OpenMP-matrix-multiplication.txt";
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
    // randomizing values
    srand(time(0));
 
    fillMatrix(matrixA);
    fillMatrix(matrixB);

    // starting timer
    auto start = high_resolution_clock::now();

    // OpenMP matrix multiplicaiton
    multiplyMatrix_openmp();

    // stopping timer
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    // printing output time
    cout << "Time taken for OpenMP matrix multiplication: " << duration.count() << " microseconds"<< endl;

    // saving output to file
    saveMatrixFile(matrixC);

    return 0;
}
