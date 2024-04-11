#include "Window.h"

#include "WindowsMessageMap.h"
#include "ChilWin.h"
#include "../ResourceFiles/Resource.h"
#include <WinUser.h>
#include <sstream>

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept: Chip8Exception(line, file), hr(hr)
{
}

const char* Window::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << '\n'
    << "[Error Code] " << GetErrorCode() << '\n'
    << "[Description] " << GetErrorString() << '\n'
    << GetOriginString();
    whatBuffer = oss.str();

    return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
    return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hResult) noexcept
{
    char* pMsgBuf = nullptr;
    DWORD msgLen = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        hResult,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf),
        0,
        nullptr);

    if(msgLen == 0)
    {
        return "Unidentified error code";
        
    }

    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);

    return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
    return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
    return TranslateErrorCode(hr);
}

Window::WindowClass::WindowClass() noexcept : hInst( GetModuleHandle(nullptr))
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = HandleMsgSetup;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetInstance();
    wcex.hIcon = LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_CHIP8EMU));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = GetName();
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndClassName, GetInstance());
}

const LPCWSTR Window::WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

Window::Window(int width, int height, const LPCWSTR name) : width(width), height(height)
{

    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;

    // Change the boolean to TRUE whenever we add a menu to the window
    
    if(!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
    {
        throw WND_LAST_EXCEPT();
    }

    HWND hWnd = CreateWindowEx(
        0,
        WindowClass::GetName(),
        name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        WindowClass::GetInstance(),
        // Providing a reference to this class that will be passed to WN_NCCREATE
        this);

    if(!hWnd)
    {
        throw WND_LAST_EXCEPT();
    }
    
    ShowWindow(hWnd,  SW_SHOWDEFAULT);

}

Window::~Window()
{
    DestroyWindow(hWnd);
}

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    if(message == WM_NCCREATE)
    {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

        // set WinAPI-manged user data to store ptr to window class
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

        // set message proc to normal (non-setup) handler
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

        return pWnd->HandleMsg(hWnd, message, wParam, lParam);
    } else {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT Window::HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    auto pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hWnd, message, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    static WindowsMessageMap mm;
    OutputDebugStringA(mm(message, lParam, wParam).c_str());
    switch (message)
    {
    case WM_COMMAND:
        {
            // int wmId = LOWORD(wParam);
            // // Parse the menu selections:
            // switch (wmId)
            // {
            // case IDM_ABOUT: DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            //     break;
            // case IDM_EXIT:
            //     DestroyWindow(hWnd);
            //     break;
            // default:
            //     return DefWindowProc(hWnd, message, wParam, lParam);
            // }
        }
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if(!(lParam & KF_REPEAT) || kb.AutoRepeatIsEnabled())
        {
            kb.onKeyPressed(static_cast<unsigned char>(wParam));
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        kb.onKeyReleased(static_cast<unsigned char>(wParam));
        break;
    case WM_CHAR:
        kb.OnChar(static_cast<char>(wParam));
        break;
    case WM_KILLFOCUS:
        kb.ClearState();
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    default:
        break;
    }

    return (INT_PTR)FALSE;
}

