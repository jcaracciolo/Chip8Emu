#include "MyTimer.h"

#include <chrono>

using namespace std::chrono;

MyTimer::MyTimer()
{
    last = steady_clock::now();
}

float MyTimer::Mark()
{
    const auto old = last;
    last = steady_clock::now();
    pausedTimestamp = resumedTimestamp = last;
    const duration<float> frameTime = last - old;
    return frameTime.count();
}

float MyTimer::Peek()
{
    if(isPaused)
    {
        auto pausedTime = duration<float>(steady_clock::now() - pausedTimestamp).count();
        return duration<float>(steady_clock::now() - last).count() - pausedTime;
    } else
    {
        auto pausedTime = duration<float>(steady_clock::now() - pausedTimestamp).count();
        auto resumeTime = duration<float>(steady_clock::now() - resumedTimestamp).count();
        return duration<float>(steady_clock::now() - last).count() - (resumeTime - pausedTime);

    }
}

void MyTimer::Pause()
{
    if(!isPaused)
    {
        isPaused = true;
        pausedTimestamp = steady_clock::now();
    }
}

void MyTimer::Resume()
{
    if(isPaused)
    {
        isPaused = false;
        resumedTimestamp = steady_clock::now();
    }
}
