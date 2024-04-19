#include "App.h"

#include <thread>

#include "Window.h"]
#include "Keyboard.h"
#include "FileManager.h"

App::App(LPCWSTR title, int width, int height): c8Keyboard(emu.keypad), wnd(title, width, height, c8Keyboard) 
{
}

int App::Run()
{
    wnd.Gfx().ClearBuffer(0, 0, 0);


    C8_BYTE* file;
    auto size = FileManager::ReadRom("roms/TETRIS", &file);
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
