#include "Timer.h"

C8_BYTE Timer::Get()
{
    auto timeSinceSet2 =timer.Peek();

    auto timeSinceSet = static_cast<C8_BYTE>(timer.Peek() * 60);
    if(value < timeSinceSet)
    {
        return 0;
    }
    return value - timeSinceSet;
}

void Timer::Set(C8_BYTE setTo)
{
    value=setTo;
    timer.Mark();
}
