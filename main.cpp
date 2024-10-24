#include <iostream>
#include <fstream>
#include <string>
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

// Test Code
int main() {
    // char fileName[100]; also works and * is not required because arrays in C++ decay into pointers when passed to functions.
    char *fileName = new char[100];  // Assuming file name length won't exceed 100 characters
    char *data = nullptr;
    size_t numOfBytes = 0;

    // Prompt the user for the file name
    cout << "Enter the file name: ";
    //cin.getline(fileName, 100);
    cin >> fileName;

    // Call the function to transfer file data to memory
    fileToMemoryTransfer(fileName, &data, numOfBytes);

    // Output the number of bytes read (for verification)
    cout << "Number of bytes read: " << numOfBytes << endl;

    // Free the allocated memory
    delete[] fileName;
    delete[] data;

    return 0;
}