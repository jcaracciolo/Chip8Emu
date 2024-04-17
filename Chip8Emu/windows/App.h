#pragma once

#include "../utils/MyTimer.h"
#include "Window.h"
#define MAX_LOADSTRING 100

class App
{
public:
    App(LPCWSTR title, int height, int width);
    int Run();

private:
    void DrawFrame();

private:
    Window wnd;
    MyTimer timer;
    Emulator emu;
};
