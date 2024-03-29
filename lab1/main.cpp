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
struct LineData {
    double x;
    double pi_x;
    double delta_x;
};

int searchInChunk(const vector<string>& chunk, const double value, int threadId) {
    int lineNumber = 0;
    double piValue = -1; // Initialize piValue to -1
    double nextValue = 0;
    bool found = false;

    // for(auto& line : chunk) cout << line << endl;    the input is correct
    
    for (const auto& line : chunk) {
        // initialize a struct LineData for each line
        
        lineNumber++;
        istringstream iss(line);
        double currentValue;

        if(!(iss >> currentValue >> nextValue)){
            return -1;
        }
        
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

int main(int argc, char* argv[]) {
    std::string filename = "test.txt";

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_chunks> <value_to_search>" << std::endl;
        return 1;
    }

    int number_of_chunks = std::stoi(argv[1]);
    double value_to_search = std::stoi(argv[2]);

    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> file_chunks;

    // Read the file and storing it into a vector of strings
    while (std::getline(file, line)) {
        file_chunks.push_back(line);
    }
    file.close();

    // chunk size 
    int chunk_size = std::ceil(file_chunks.size() / static_cast<double>(number_of_chunks));

    std::vector<std::thread> threads;

    // runinng the threads
    for (int i = 0; i < number_of_chunks; i++) {
        int start_index = i * chunk_size;
        int end_index = std::min((i + 1) * chunk_size, static_cast<int>(file_chunks.size()));
        vector<string> chunk(file_chunks.begin() + start_index, file_chunks.begin() + end_index);
        threads.emplace_back(searchInChunk, chunk, value_to_search, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
