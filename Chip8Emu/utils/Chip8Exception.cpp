#include "Chip8Exception.h"

#include <ostream>
#include <sstream>

Chip8Exception::Chip8Exception(int line, const char* file): line(line), file(file)
{
}

const char* Chip8Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << '\n' << GetOriginString();
    whatBuffer = oss.str();

    return whatBuffer.c_str();
}

const char* Chip8Exception::GetType() const noexcept
{
    return "Chip8 Exception";
}

int Chip8Exception::GetLine() const noexcept
{
    return line;
}

const std::string& Chip8Exception::GetFile() const noexcept
{
    return file;
}

std::string Chip8Exception::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << file << std::endl << "[Line] " << line;
    return  oss.str();
}
