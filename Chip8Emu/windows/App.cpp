#include "App.h"

#include <iomanip>
#include <ostream>
#include <sstream>

#include "LightWindows.h"
#include "Window.h"
#include "../utils/Keyboard.h"

App::App(LPCWSTR title, int height, int width): wnd(title, height, width)
{
}

int App::Run()
{
    while(true)
    {
        if(const auto ecode = Window::ProcessMessage())
        {
            return *ecode;
        }

        DrawFrame();
    }
}

void App::DrawFrame()
{
    const float c = sin( timer.Peek()) / 2.0f + 0.5f;
    wnd.Gfx().ClearBuffer(c,c/4,c/2);
    wnd.Gfx().EndFrame();
}
