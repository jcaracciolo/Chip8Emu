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
    try
    {
        while(true)
        {
            if(const auto ecode = Window::ProcessMessage())
            {
                return *ecode;
            }

            DrawFrame();
        }

    } catch (const Chip8Exception& e)
    {
        MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK |MB_ICONEXCLAMATION);
    } catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    } catch ( ... )
    {
        MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return -1;
}

void App::DrawFrame()
{
    const float t = timer.Peek();
    std::ostringstream oss;
    oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
    wnd.SetTitle(oss.str());
}
