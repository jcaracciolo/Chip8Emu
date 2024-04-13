#pragma once

#include "../LightWindows.h"
#include <d3d11.h>

#include "../../utils/Chip8Exception.h"

class Graphics
{
public:
    // Constructors
    Graphics(HWND hWnd);
    ~Graphics();
    Graphics(const Graphics& other) = delete;
    Graphics(Graphics&& other) noexcept = delete;
    Graphics& operator=(const Graphics& other) = delete;
    Graphics& operator=(Graphics&& other) noexcept = delete;

    // API
    void EndFrame();
    void ClearBuffer(float red, float green, float blue);


public:
    class Exception: Chip8Exception
    {
        using Chip8Exception::Chip8Exception;
    };

    class HrException: public Chip8Exception
    {
    public:
        HrException(int line, const char* file, HRESULT hr) noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;
        static std::string TranslateErrorCode(HRESULT hResult) noexcept;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
    private:
        HRESULT hr;
    };

    class DeviceRemovedException: public HrException
    {
        using HrException::HrException;
    public:
        const char* GetType() const noexcept override;
    };

private:
    ID3D11Device* pDevice = nullptr;
    IDXGISwapChain* pSwap = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* pTarget = nullptr;
    
};
