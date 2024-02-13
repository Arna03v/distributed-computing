#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <sstream>
using namespace std;

std::mutex mtx;

int searchInChunk(const vector<std::string>& chunk, const double value, int threadId) {
    int lineNumber = 0;
    double piValue = -1; // Initialize piValue to -1
    int nextValue = 0;
    bool found = false;

    for (const auto& line : chunk) {
        lineNumber++;
        std::istringstream iss(line);
        double currentValue;
        iss >> currentValue;
        if (currentValue == value) {
            if (iss >> nextValue) {
                found = true;
                iss >> piValue; // Assign the value of pi(x) to piValue
            }
            break;
        }
    }
    // now we have the correct line
    
    std::lock_guard<std::mutex> lock(mtx);  // unlocked when this mutex goes out of scope

    if (found) {
        std::cout << "Thread " << threadId << ": Found value " << value << " with next value " << nextValue << std::endl;
    } else {
        std::cout << "Thread " << threadId << ": Value " << value << " not found" << std::endl;
    }

    return piValue; // Return pi(x) if found, -1 otherwise
}

int main() {
    std::string filename = "master_file.txt";
    int number_of_chunks; 
    cin >> number_of_chunks;
    
    int value_to_search; cin >> value_to_search;

    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> file_chunks;

    // Read the file and divide it into chunks
    while (std::getline(file, line)) {
        file_chunks.push_back(line);
    }
    file.close();

    int chunk_size = std::ceil(file_chunks.size() / static_cast<double>(number_of_chunks));

    std::vector<std::thread> threads;

    for (int i = 0; i < number_of_chunks; i++) {
        int start_index = i * chunk_size;
        int end_index = std::min((i + 1) * chunk_size, static_cast<int>(file_chunks.size()));
        std::vector<std::string> chunk(file_chunks.begin() + start_index, file_chunks.begin() + end_index);
        threads.emplace_back(searchInChunk, chunk, value_to_search, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
