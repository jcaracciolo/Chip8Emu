#pragma once
#include "ChilWin.h"
#include "Chip8Exception.h"

class Window
{
public:
    class Exception : public Chip8Exception
    {
    public:
        Exception(int line, const char* file, HRESULT hr) noexcept;
        const char* what() const noexcept override;
        virtual const char* GetType() const noexcept override;
        static std::string TranslateErrorCode(HRESULT hResult) noexcept;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;

    private:
        HRESULT hr;
    };

private:
    class WindowClass
    {
    public:
        static const LPCWSTR GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;

    private:
        WindowClass() noexcept;
        ~WindowClass();
        WindowClass(const WindowClass& other) = delete;
        WindowClass(WindowClass&& other) noexcept = delete;
        WindowClass& operator=(const WindowClass& other) = delete;
        WindowClass& operator=(WindowClass&& other) noexcept = delete;
        static constexpr const LPCWSTR wndClassName = L"Chip8Emu Window";
        static WindowClass wndClass;
        HINSTANCE hInst;
    };

public:
    Window(int width, int height, const LPCWSTR name) noexcept;
    ~Window();
    Window(const Window& other) = delete;
    Window(Window&& other) noexcept = delete;
    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other) noexcept = delete;

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

private:
    int width;
    int height;
    HWND hWnd;
};

#define WND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr);
