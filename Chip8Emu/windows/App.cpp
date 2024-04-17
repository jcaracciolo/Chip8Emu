#include "App.h"

#include <iomanip>

#include "LightWindows.h"
#include "Window.h"
#include "Keyboard.h"
#include "FileManager.h"

App::App(LPCWSTR title, int width, int height): wnd(title, width, height), keypad(wnd.kb),
                                                emu([&](C8_BYTE key) { return keypad.GetKeyState(key); })
{
}

int App::Run()
{
    wnd.Gfx().ClearBuffer(0, 0, 0);


    C8_BYTE* file;
    auto size = FileManager::ReadRom("roms/PUZZLE", &file);
    // TODO check size is correct
    emu.LoadRom(file, size);

    while (true)
    {
        if (const auto ecode = Window::ProcessMessage())
        {
            return *ecode;
        }

        if (emu.Tick())
        {
            DrawFrame();
        }
    }
}

void App::DrawFrame()
{
    wnd.Gfx().ClearBuffer(0, 0, 0);

    auto screen = emu.getScreen();

    for (int y = 0; y < CHIP8_HEIGHT; y++)
    {
        for (int x = 0; x < CHIP8_WIDTH; x++)
        {
            if (screen[y].test(x))
            {
                wnd.Gfx().SetPixelOn(x, y);
            }
        }
    }

    wnd.Gfx().EndFrame();
}
