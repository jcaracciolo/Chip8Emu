#pragma once
#include <chrono>

class MyTimer
{
public:
    MyTimer();
    float Mark();
    float Peek();
    void Pause();
    void Resume();

private:
    std::chrono::time_point<std::chrono::steady_clock> last;
    std::chrono::time_point<std::chrono::steady_clock> pausedTimestamp;
    std::chrono::time_point<std::chrono::steady_clock> resumedTimestamp;
    bool isPaused;
};
