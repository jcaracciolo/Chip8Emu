#pragma once
#include <chrono>

class MyTimer
{
public:
    MyTimer();
    float Mark();
    float Peek();

private:
    std::chrono::time_point<std::chrono::steady_clock> last;
};
