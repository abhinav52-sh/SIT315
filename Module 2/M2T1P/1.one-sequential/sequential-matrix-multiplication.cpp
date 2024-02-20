#include <iostream>
#include <cstdlib>
#include <fstream>
#include <time.h>
#include <chrono>

#define matrixSize 100

using namespace std;
using namespace std::chrono;

// intialize matrix with random values between 0 and 9
void fillMatrix(int** arr, int size)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            arr[i][j] = rand() % 10;
        }
    }
}

// multiply two matrices
void multiplyMatrix(int** arrA, int** arrB, int** arrC, int size)
{
    for(int i = 0; i < matrixSize; i++)
    {
        for(int j = 0; j < matrixSize; j++)
        {
            arrC[i][j] = 0;
            for(int k = 0; k < matrixSize; k++)
            {
                arrC[i][j] += arrA[i][k] *  arrB[k][j];
            }
        }
    }
}

// save matrix elements value to a file
void saveMatrixFile(int** arr, int size)
{
    string fileName = "matrix-multiplication.txt";
    ofstream file(fileName);
    if(file.is_open()) {
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
            {
                file << arr[i][j] << "   ";
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
    srand(time(0)); 

    // memory allocation
    int** arrA = new int*[matrixSize];
    int** arrB = new int*[matrixSize];
    int** arrC = new int*[matrixSize];

    for(int i = 0; i < matrixSize; i++)
    {
        arrA[i] = new int[matrixSize];
        arrB[i] = new int[matrixSize];
        arrC[i] = new int[matrixSize];
    }

    fillMatrix(arrA, matrixSize);
    fillMatrix(arrB, matrixSize);

    // Recording start time
    auto start = high_resolution_clock::now();    

    multiplyMatrix(arrA, arrB, arrC, matrixSize);

    // Recording stop time
    auto stop = high_resolution_clock::now();

    // Calculating and printing the duration time to perform the vector addition
    auto duration = duration_cast<microseconds>(stop - start);


    saveMatrixFile(arrC, matrixSize);
    
    cout << "Time taken for matrix multiplication: " << duration.count() << " microseconds"<< endl;

    // memory deallocation
    for (int i = 0; i < matrixSize; i++) {
        delete[] arrA[i];
        delete[] arrB[i];
        delete[] arrC[i];
    }

    delete[] arrA;
    delete[] arrB;
    delete[] arrC;


}