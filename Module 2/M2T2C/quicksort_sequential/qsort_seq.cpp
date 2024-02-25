/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Task: M2T2C
* Unit: SIT315
*/

#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define VECTOR_SIZE 2000000

//Ffilling a vector with random integers 
void fillVector(vector<int>& vect, int size)
{   
    for(int i = 0; i < size; i++)
    {
        vect.push_back(rand() % 100);
    }
}

// Printign all vector elements
void printVector(vector<int> vect)
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
    for(int j = start; j <= end; j++)
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

// Recursive function to perform QuickSort
void quickSort(vector<int>& vect, int start, int end)
{
    // Base case: if the partition has more than one element
    if(start < end)
    {
        // Finding the partition index
        int partition = sortPartition(vect, start, end);

        // Recursively sorting the sub-vectors before and after the partition
        quickSort(vect, start, partition-1);
        quickSort(vect, partition+1, end);
    }
}

int main()
{
    srand(time(0));

    // Creating and filling a vetor with random data
    vector<int> data;
    fillVector(data, VECTOR_SIZE);

    // Measuring the starting time
    auto start = high_resolution_clock::now();

    // Performing QuickSort on the vector
    quickSort(data, 0, data.size()-1);

    // Measuring the ending time
    auto end = high_resolution_clock::now();

    // Calculating the duration of the sorting process
    duration<double> durationTime = end - start;

    // Printing the sorted vector
    // printVector(data);

    // Printing the execution time
    cout << "Execution Time: " << durationTime.count() << " seconds" << endl;

    return 0;
}


