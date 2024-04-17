﻿#pragma once
#include "Instructions.h"
#include "../utils/MyTimer.h"

class Timer
{
public:
    C8_BYTE Get();
    void Set(C8_BYTE value);

private:
    C8_BYTE value = 0;
    MyTimer timer = {};
};
