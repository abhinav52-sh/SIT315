/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Task: M2T2C
* Unit: SIT315
*/

#include <iostream>
#include <vector>
#include <chrono>
#include <pthread.h>

using namespace std;
using namespace std::chrono;

#define VECTOR_SIZE 100000
#define NUM_THREADS 4

// Struct to hold information for each thread
struct ThreadInfo {
    vector<int>* vect; // Pointer to the vector
    int start;         // Start index for the thread's partition
    int end;           // End index for the thread's partition
};

// Filling a vector with random integers
void fillVector(vector<int>& vect, int size)
{   
    for(int i = 0; i < size; i++)
    {
        vect.push_back(rand() % 100);
    }
}

// Printing vector elements
void printVector(const vector<int>& vect)
{
    for(int i = 0; i < vect.size(); i++)
    {
        if(i % 10 == 0 && i > 0) cout << endl;
        cout << vect[i] << " ";
    }
    cout << endl;
}

// Performing the partition step of QuickSort
int sortPartition(vector<int>& vect, int start, int end)
{
    int pivot = vect[end];
    int i = start - 1;

    // Iterating through the elements, rearranging them around the pivot
    for(int j = start; j < end; j++)
    {   
        if(vect[j] < pivot)
        {
            i++;
            swap(vect[i], vect[j]);
        }
    }

    // Placing the pivot in its correct position
    swap(vect[i+1], vect[end]);
    return (i+1);
}

// performing QuickSort on a vector
void quickSort(vector<int>& vect, int start, int end)
{
    if(start < end)
    {
        int partition = sortPartition(vect, start, end);

        // Recursively sort the sub-vectors before and after the partition
        quickSort(vect, start, partition-1);
        quickSort(vect, partition+1, end);
    }
}

// Function executed by each thread to perform QuickSort on its partition
void* quickSortThread(void* arg)
{
    ThreadInfo* info = static_cast<ThreadInfo*>(arg);
    quickSort(*(info->vect), info->start, info->end);
    return nullptr;
}

// Parallelizing QuickSort using pthreads
void quickSortParallel(vector<int>& vect, int start, int end, int numThreads)
{
    if(start < end)
    {
        int partition = sortPartition(vect, start, end);

        pthread_t threads[numThreads];
        ThreadInfo threadInfo[numThreads];

        int range = (end - start + 1) / numThreads;
        for (int i = 0; i < numThreads; ++i)
        {
            // Assigning partition ranges to each thread
            threadInfo[i] = {&vect, start + i * range, start + (i + 1) * range - 1};
            // Creating threads and executing quickSortThread function
            pthread_create(&threads[i], nullptr, quickSortThread, &threadInfo[i]);
        }

        // Waiting for all threads to complete
        for (int i = 0; i < numThreads; ++i)
        {
            pthread_join(threads[i], nullptr);
        }
    }
}

int main()
{
    srand(time(0));

    // Creating and filling a vetor with random data
    vector<int> data;
    fillVector(data, VECTOR_SIZE);

    //  Measuring starting time
    auto start = high_resolution_clock::now();

    // Use threads for parallel quicksort
    quickSortParallel(data, 0, data.size()-1, NUM_THREADS);

    // Measuring stopping time
    auto end = high_resolution_clock::now();

    // Calculating the duration of the sorting process
    duration<double> durationTime = end - start;

    // Printing the sorted vector
    // printVector(data);

    // Printing the execution time
    cout << "Execution Time: " << durationTime.count() << " seconds" << endl;

    return 0;
}
