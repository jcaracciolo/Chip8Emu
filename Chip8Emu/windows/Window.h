#pragma once
#include "LightWindows.h"
#include "../utils/Chip8Exception.h"
#include "Keyboard.h"
#include "graphics/Graphics.h"
#include <memory.h>
#include <memory>

#include "Keyboard.h"

class Window
{
public:
    Window(const LPCWSTR name, int width, int height);
    ~Window();
    Window(const Window& other) = delete;
    Window(Window&& other) noexcept = delete;
    Window& operator=(const Window& other) = delete;
    Window& operator=(Window&& other) noexcept = delete;

    
    void SetTitle(const std::string& title);
    static std::optional<int> ProcessMessage();

    Keyboard kb;
    Graphics& Gfx();

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

    class NoGraphicsException : public Chip8Exception
    {
    public:
        using Chip8Exception::Chip8Exception;
        const char* GetType() const noexcept override;
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

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

    int width;
    int height;
    HWND hWnd;
    std::unique_ptr<Graphics> pGfx;
};

#define WND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr);
#define WND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError());
#define WND_NOGFX_EXCEPT() Window::NoGraphicsException(__LINE__, __FILE__);

