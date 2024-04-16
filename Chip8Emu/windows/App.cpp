#include "App.h"

#include <iomanip>

#include "LightWindows.h"
#include "Window.h"
#include "../utils/Keyboard.h"

App::App(LPCWSTR title, int width, int height): wnd(title, width, height)
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
        wnd.Gfx().ClearBuffer(0,0,0);

        DrawFrame();
    }
}

void App::DrawFrame()
{
    const float c = sin( timer.Peek()) / 2.0f + 0.5f;
    static int a = 0;
    a++;

    for(int y=0; y< CHIP8_HEIGHT; y++)
    {
        for(int x=0; x<CHIP8_WIDTH; x++)
        {
            auto pos = x + y * CHIP8_WIDTH;
        
            if (pos < a) {
                wnd.Gfx().SetPixelOn(x, y);
            }
           
        }
    }
    
    wnd.Gfx().EndFrame();
}
