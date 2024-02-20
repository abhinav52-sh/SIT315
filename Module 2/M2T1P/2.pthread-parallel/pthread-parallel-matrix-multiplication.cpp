#include <iostream>
#include <cstdlib>
#include <fstream>
#include <time.h>
#include <chrono>
#include <pthread.h>

#define matrixSize 100
#define numThreads 4  

using namespace std;
using namespace std::chrono;

struct ThreadData {
    int id;
    int** A;
    int** B;
    int** C;
    int size;
};

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

// function for parallel multiplication for each thread
void* parallelMatrixMultiplication(void* data) {
    ThreadData* td = static_cast<ThreadData*>(data);
    int startRow = (td->size / numThreads) * td->id;
    int endRow = (td->size / numThreads) * (td->id + 1);

    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < td->size; ++j) {
            td->C[i][j] = 0;
            for (int k = 0; k < td->size; ++k) {
                td->C[i][j] += td->A[i][k] * td->B[k][j];
            }
        }
    }

    pthread_exit(nullptr);
}

int main() {
    srand(time(0)); 

    // Memory allocation
    int** arrA = new int*[matrixSize];
    int** arrB = new int*[matrixSize];
    int** arrC = new int*[matrixSize];

    for(int i = 0; i < matrixSize; i++) {
        arrA[i] = new int[matrixSize];
        arrB[i] = new int[matrixSize];
        arrC[i] = new int[matrixSize];
    }

    fillMatrix(arrA, matrixSize);
    fillMatrix(arrB, matrixSize);

    // Create threads and data structures
    pthread_t threads[numThreads];
    ThreadData td[numThreads];

    // Recording start time
    auto start = high_resolution_clock::now();

    // Launch threads
    for (int i = 0; i < numThreads; ++i) {
        td[i].id = i;
        td[i].A = arrA;
        td[i].B = arrB;
        td[i].C = arrC;
        td[i].size = matrixSize;

        pthread_create(&threads[i], nullptr, parallelMatrixMultiplication, &td[i]);
    }

    // Join threads
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Recording stop time
    auto stop = high_resolution_clock::now();

    // Calculating and printing the duration time
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken for parallel matrix multiplication: " << duration.count() << " microseconds" << endl;

    // Save matrix to file
    saveMatrixFile(arrC, matrixSize);

    // Memory deallocation
    for (int i = 0; i < matrixSize; i++) {
        delete[] arrA[i];
        delete[] arrB[i];
        delete[] arrC[i];
    }

    delete[] arrA;
    delete[] arrB;
    delete[] arrC;

    return 0;
}
