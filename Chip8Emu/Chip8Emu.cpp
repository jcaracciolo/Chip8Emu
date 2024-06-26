// Chip8Emu.cpp : Defines the entry point for the application.
//

#include "windows/LightWindows.h"
#include "Chip8Emu.h"

#include "Engine/Instructions.h"
#include "windows/App.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    std::unique_ptr<App> app; 
    try
    {
        app = std::make_unique<App>(L"Chip8Emu - Best emu", WINDOW_WIDTH, WINDOW_HEIGHT);
        return app->Run();
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
