#include <iostream>
#include <cstdlib>
#include <fstream>
#include <time.h>
#include <chrono>

#define matrixSize 400

using namespace std;
using namespace std::chrono;

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
            matrix[i][j] = rand() % 10;
        }
    }
}

// multiply two matrices
void multiplyMatrix(int matrixA[][matrixSize], int matrixB[][matrixSize], int matrixC[][matrixSize])
{
    for(int i = 0; i < matrixSize; i++)
    {
        for(int j = 0; j < matrixSize; j++)
        {
            matrixC[i][j] = 0;
            for(int k = 0; k < matrixSize; k++)
            {
                matrixC[i][j] += matrixA[i][k] *  matrixB[k][j];
            }
        }
    }
}

// save matrix elements value to a file
void saveMatrixFile(int matrix[][matrixSize])
{
    string fileName = "sequential-matrix-multiplication.txt";
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
    // randomize values for filling matrices
    srand(time(0)); 

    fillMatrix(matrixA);
    fillMatrix (matrixB);

    // Recording start time
    auto start = high_resolution_clock::now();    

    // running sequential matrix multiplication
    multiplyMatrix(matrixA, matrixB, matrixC);

    // Recording stop time
    auto stop = high_resolution_clock::now();

    // Calculating and printing the duration time to perform the vector addition
    auto duration = duration_cast<microseconds>(stop - start);

    // saving output to file
    saveMatrixFile(matrixC);
    
    cout << "Time taken for sequential matrix multiplication: " << duration.count() << " microseconds"<< endl;

    return 0;
}

