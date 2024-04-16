#pragma once

#include "../LightWindows.h"
#include "../../utils/Chip8Exception.h"
#include "../../Chip8Emu.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <vector>

#include "DxgiInfoManager.h"


// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_CUSTOM_EXCEPT(string) Graphics::Exception( __LINE__,__FILE__,{(string)} )
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::Exception( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

class Graphics
{
public:
    // Constructors
    Graphics(HWND hWnd);
    ~Graphics() = default;
    Graphics(const Graphics& other) = delete;
    Graphics(Graphics&& other) noexcept = delete;
    Graphics& operator=(const Graphics& other) = delete;
    Graphics& operator=(Graphics&& other) noexcept = delete;

    // API
    struct Vertex
    {
        UINT16 x;
        UINT16 y;
    };
    
    void EndFrame();
    void ClearBuffer(float red, float green, float blue);
    void ClearScreen();
    void SetPixelOn(int x, int y);

private:
    void SetupPipeline();
    void SetupVertexShader();
    void SetupPixelShader();
    void DrawPixels();

public:
    class Exception : public Chip8Exception
    {
    public:
        Exception(int line,const char* file, std::vector<std::string> infoMsgs = {} ) noexcept;
        std::string GetErrorInfo() const noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;

    private:
        std::string info;
    };
    
    class HrException : public Exception
    {
    public:
        HrException(int line,const char* file,HRESULT hr,std::vector<std::string> infoMsgs = {} ) noexcept;
        const char* what() const noexcept override;
        const char* GetType() const noexcept override;
        static std::string TranslateErrorCode(HRESULT hResult) noexcept;
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;

    private:
        HRESULT hr;
    };

    class DeviceRemovedException : public HrException
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

    // Buffers
    std::vector<UINT32> indices = {};
    bool pipelineSetup = false;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
    
#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
};

