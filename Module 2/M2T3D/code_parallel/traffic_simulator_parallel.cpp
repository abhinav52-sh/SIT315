/*
 * Author: Abhinav Sharma
 * Student Id: 2210994752
 * Task: M2T3D
 * Unit: SIT315
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iomanip> // for std::put_time

using namespace std;

struct TrafficData
{
    std::time_t timestamp;
    int lightId;
    int numCars;
};

std::queue<TrafficData> dataQueue;
bool producerFinished = false; // Flag indicating that the producer has finished reading the input file

bool operator<(const TrafficData &left, const TrafficData &right)
{
    return left.timestamp < right.timestamp;
}

// Function to simulate traffic data production
void producer(std::ifstream &inputFile, std::queue<TrafficData> &dataQueue,
              std::mutex &dataMutex, std::condition_variable &dataCV)
{
    std::string line;
    bool skipFirstLine = true; // Flag to skip the first line
    while (std::getline(inputFile, line))
    {
        if (skipFirstLine)
        {
            skipFirstLine = false;
            continue;
        }

        std::stringstream ss(line);
        std::string timestampStr, lightIdStr, numCarsStr;
        std::getline(ss, timestampStr, ',');
        std::getline(ss, lightIdStr, ',');
        std::getline(ss, numCarsStr, ',');
        TrafficData trafficData{
            .timestamp = std::stol(timestampStr),
            .lightId = std::stoi(lightIdStr),
            .numCars = std::stoi(numCarsStr)};
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            dataQueue.push(trafficData);
        }
        dataCV.notify_one();
    }

    // Notify the consumer that the producer has finished reading the input file
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        producerFinished = true;
    }
    dataCV.notify_one();
}

// Function to simulate traffic data consumption and reporting
void consumer(const int topN, std::queue<TrafficData> &dataQueue,
              std::mutex &dataMutex, std::condition_variable &dataCV)
{
    std::unordered_map<int, int> carCounts;
    std::time_t currentHour = dataQueue.front().timestamp / 3600 * 3600;
    while (true)
    {
        std::unique_lock<std::mutex> lock(dataMutex);
        dataCV.wait(lock, [&dataQueue]() { return !dataQueue.empty() || producerFinished; });

        while (!dataQueue.empty() && dataQueue.front().timestamp - currentHour < 3600)
        {
            carCounts[dataQueue.front().lightId] += dataQueue.front().numCars;
            dataQueue.pop();
        }

        // Report top N congested traffic lights if the current hour has ended
        if (!dataQueue.empty() && dataQueue.front().timestamp - currentHour >= 3600)
        {
            std::vector<std::pair<int, int>> lightCounts(carCounts.begin(), carCounts.end());
            std::sort(lightCounts.begin(), lightCounts.end(), [](const std::pair<int, int> &left, const std::pair<int, int> &right)
            {
                return left.second > right.second;
            });

            // Display date and time in numeric format
            std::tm *tmTime = std::localtime(&currentHour);
            // std::cout << "Hour " << std::put_time(tmTime, "%d-%m-%Y on %H:%M:%S") << std::endl;
            std::cout << "\n************************************";
            std::cout << "\nHour " << std::put_time(tmTime, "%d-%m-%Y on %H:%M:%S");
            std::cout << "\nTop " << topN << " Congested Traffic Lights:";
            std::cout << "\n************************************\n";

            for (int i = 0; i < std::min(topN, (int)lightCounts.size()); i++)
            {
                std::cout << "Traffic Light " << lightCounts[i].first << ": " << lightCounts[i].second << " cars" << std::endl;
            }
            std::cout << std::endl;

            // Reset car counts for the next hour
            carCounts.clear();
            currentHour = dataQueue.front().timestamp - (dataQueue.front().timestamp % 3600);
        }

        // Check if the producer has finished and the queue is empty, then break out of the loop
        if (producerFinished && dataQueue.empty())
        {
            break;
        }

        lock.unlock();
    }
}

int main()
{
    const std::string dataFile = "sample_data_traffic.txt";
    const int topN = 5;

    std::ifstream inputFile(dataFile);
    if (!inputFile.is_open())
    {
        std::cerr << "Error opening file: " << dataFile << std::endl;
        return 1;
    }

    // Create queue and synchronization objects
    std::mutex dataMutex;
    std::condition_variable dataCV;

    // Create producer and consumer threads
    std::thread producerThread(producer, std::ref(inputFile), std::ref(dataQueue), std::ref(dataMutex), std::ref(dataCV));
    std::thread consumerThread(consumer, topN, std::ref(dataQueue), std::ref(dataMutex), std::ref(dataCV));

    // Wait for threads to finish
    producerThread.join();
    consumerThread.join();

    return 0;
}
