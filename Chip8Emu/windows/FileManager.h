#pragma once
#include <memory>

#include "../Engine/Emulator.h"

class FileManager
{
public:
    static long long ReadRom(std::string filePath, C8_BYTE** memory);
};
