// Chip8Emu.cpp : Defines the entry point for the application.
//

#include "windows/LightWindows.h"
#include "Chip8Emu.h"
#include "windows/App.h"

int window_height = 460;
int window_width = 480;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    try
    {
        return App{L"Chip8Emu - Best emu", window_height, window_width}.Run();
    }
    catch (const Chip8Exception& e)
    {
        MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    } catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    } catch (...)
    {
        MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return -1;
}
