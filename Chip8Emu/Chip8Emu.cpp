// Chip8Emu.cpp : Defines the entry point for the application.
//

#include "utils/ChilWin.h"
#include "utils/Window.h"
#include "utils/Keyboard.h"
#include "Chip8Emu.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst; // current instance
WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

int window_height = 460;
int window_width = 480;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHIP8EMU, szWindowClass, MAX_LOADSTRING);

    try
    {
        Window wnd{420,420, L"Chip8Emu" };
        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHIP8EMU));

        MSG msg;

        // Main message loop:
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                // No need for this unless we need WM_CHAR
                //TranslateMessage(&msg);
                DispatchMessage(&msg);
                if(wnd.kb.KeyIsPressed(VK_SPACE))
                {
                    MessageBoxA(nullptr, "SOMETHING", "Whatever", MB_OK | MB_ICONHAND);
                }
            }
        }


        return static_cast<int>(msg.wParam);
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
