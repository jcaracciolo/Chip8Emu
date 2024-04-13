#pragma once

#include "../LightWindows.h"
#include "../../utils/Chip8Exception.h"

#include <d3d11.h>
#include <wrl.h>
#include <vector>

#include "DxgiInfoManager.h"

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
    void ClearBuffer(float red, float green, float blue) const;


public:
    class Exception: Chip8Exception
    {
        using Chip8Exception::Chip8Exception;
    };

    class HrException: public Chip8Exception
    {
    public:
        HrException(int line, const char* file, HRESULT hr, std::vector<std::string> info = {}) noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;
        static std::string TranslateErrorCode(HRESULT hResult) noexcept;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
        std::string GetErrorInfo() const noexcept;
    private:
        HRESULT hr;
        std::string info;
    };

    class DeviceRemovedException: public HrException
    {
        using HrException::HrException;
    public:
        const char* GetType() const noexcept override;
    private:
        std::string reason;
    };

private:
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;

#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
};

// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr), infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif
