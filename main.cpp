#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;

// function to transfer contents of a file into a buffer in RAM
void fileToMemoryTransfer(char *fileName, char **data, size_t &numOfBytes) {
    streampos begin, end;
    ifstream inFile(fileName, ios::in | ios::binary | ios::ate);
    if (!inFile) {
        cerr << "Cannot open " << fileName << endl;
        inFile.close();
        exit(1);
    }
    size_t size = inFile.tellg();
    char *buffer = new char[size];
    inFile.seekg(0, ios::beg);
    inFile.read(buffer, size);
    *data = buffer;
    numOfBytes = size;
}


// function to compute local histogram
void localHistogram(const char* data, size_t numOfBytes, std::atomic<long long>* globalHistogram, size_t slots) {

    for (size_t i = 0; i < slots; i++) globalHistogram[i] = 0;

    const size_t numThreads = std::thread::hardware_concurrency();
    vector<std::thread> workers;
    vector<vector<long long> > localHistograms(numThreads, vector<long long>(slots, 0));
    size_t rows = numOfBytes / numThreads;
    size_t extra = numOfBytes % numThreads;
    size_t start = 0;

    for (size_t t = 0; t < numThreads; ++t) {
        size_t end = start + rows + (t == numThreads - 1 ? extra : 0);
        workers.emplace_back([data, start, end, &localHistograms, t]() {
            for (size_t i = start; i < end; ++i) {
                unsigned char byteVal = static_cast<unsigned char>(data[i]);
                localHistograms[t][byteVal]++;
            }
            });
        start = end;
    }

    // joining all threads
    for (auto& t : workers) t.join();

    // combining all the local histograms into a single global histogram
    for (size_t t = 0; t < numThreads; ++t) {
        for (size_t i = 0; i < slots; ++i) {
            globalHistogram[i] += localHistograms[t][i];
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2 || argv[1] == nullptr) {
        cerr << "Filename not provided" << endl;
        cerr << "Usage: " << argv[0] << " <file_name>" << endl;
        return 1;
    }

    // char fileName[100]; also works and * is not required because arrays in C++ decay into pointers when passed to functions.

    char *fileName = argv[1];                                                       // Assuming file name length won't exceed 100 characters
    char *data = nullptr;                                                           // data array
    size_t numOfBytes = 0;                                                          // stored the total number of bytes in the test file
    const size_t slots = 256;                                                       // largest byte value
    std::atomic<long long>* globalHistogram = new std::atomic<long long>[slots];    // global histogram initialization

    // Call the function to transfer file data to memory
    fileToMemoryTransfer(fileName, &data, numOfBytes);

    // call the function to calculate local histogram
    localHistogram(data, numOfBytes, globalHistogram, slots);

    // output the local histogram
    cout << "Run with local histograms" << endl;
    for (size_t i = 0; i < slots; i++) {
        long long val = globalHistogram[i];
        std::cout << i << ": "<< val << "\n";
    }

    // Free the allocated memory
    delete[] data;
    data = nullptr;
    delete [] globalHistogram;
    globalHistogram = nullptr;

    return 0;
}