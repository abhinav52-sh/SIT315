/*
 * Name: Abhinav Sharma
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
#include <iomanip>
#include <sstream>

// Structure to store traffic data
struct TrafficData
{
    std::time_t timestamp;
    int light_id;
    int num_cars;
};

// Function to compare timestamps for sorting
bool isEarlier(const TrafficData &left, const TrafficData &right)
{
    return left.timestamp < right.timestamp;
}

int main()
{
    // Constants
    const std::string data_file = "sample_data_traffic.txt";
    const int top_n = 5;

    // Opening the input file
    std::ifstream infile(data_file);

    // Reading data into a vector of TrafficData
    std::vector<TrafficData> data;
    std::string line;
    while (std::getline(infile, line))
    {
        std::stringstream ss(line);
        std::string timestamp_str, light_id_str, num_cars_str;
        std::getline(ss, timestamp_str, ',');
        std::getline(ss, light_id_str, ',');
        std::getline(ss, num_cars_str, ',');
        TrafficData td{
            .timestamp = std::stol(timestamp_str),
            .light_id = std::stoi(light_id_str),
            .num_cars = std::stoi(num_cars_str)};
        data.push_back(td);
    }

    // Sorting the data by timestamp
    std::sort(data.begin(), data.end(), isEarlier);
    std::time_t current_hour = data.front().timestamp / 3600 * 3600;

    // Processing data hourly
    std::unordered_map<int, int> car_counts;
    for (const TrafficData &td : data)
    {
        if (td.timestamp - current_hour >= 3600)
        {
            // Reporting top N congested traffic lights
            std::vector<std::pair<int, int>> light_counts(car_counts.begin(), car_counts.end());
            std::sort(light_counts.begin(), light_counts.end(), [](const std::pair<int, int> &left, const std::pair<int, int> &right)
            {
                return left.second > right.second;
            });
            std::cout << "\n********************************************";
            std::cout << "\nHour: " << std::put_time(std::localtime(&current_hour), "%H:%M:%S on %m/%d/%Y");
            std::cout << "\nTop " << top_n << " Congested Traffic Lights:";
            std::cout << "\n********************************************\n";

            // Printing the top N congested traffic lights
            for (int i = 0; i < std::min(top_n, static_cast<int>(light_counts.size())); i++)
            {
                std::cout << "Traffic Light " << light_counts[i].first << ": " << light_counts[i].second << " cars\n";
            }

            // Resetting car counts for the next hour
            car_counts.clear();
            current_hour = td.timestamp - (td.timestamp % 3600);
        }
        car_counts[td.light_id] += td.num_cars;
    }

    // Reporting top N congested traffic lights for the last hour
    std::vector<std::pair<int, int>> light_counts(car_counts.begin(), car_counts.end());
    std::sort(light_counts.begin(), light_counts.end(), [](const std::pair<int, int> &left, const std::pair<int, int> &right)
    {
        return left.second > right.second;
    });

    // Printing the final report header
    std::cout << "\n****************************************";
    std::cout << "\nHour: " << std::put_time(std::localtime(&current_hour), "%H:%M:%S on %m/%d/%Y");
    std::cout << "\nTop " << top_n << " Congested Traffic Lights ->";
    std::cout << "\n****************************************\n";

    // Printing the top N congested traffic lights for the last hour
    for (int i = 0; i < std::min(top_n, static_cast<int>(light_counts.size())); i++)
    {
        std::cout << "Traffic Light " << light_counts[i].first << ": " << light_counts[i].second << " cars\n";
    }

    // Closing the input file
    infile.close();
    return 0;
}
