#include "FileManager.h"
#include <iostream>
#include <fstream>

long long FileManager::ReadRom(std::string filePath, C8_BYTE** memory)
{
    std::ifstream input(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!input.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return -1;
    }

    // Get the file size
    std::streampos size = input.tellg();
    char* memblock = new char[size];

    // Read the entire file into memory
    input.seekg(0, std::ios::beg);
    input.read(memblock, size);
    input.close();

    // Now 'memblock' contains the binary data from the file
    std::cout << "File content read into memory. Size: " << size << " bytes." << std::endl;

    
    *memory = reinterpret_cast<C8_BYTE*>(memblock);

    return size;

}
