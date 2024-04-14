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
    void EndFrame();
    void ClearBuffer(float red, float green, float blue) const;
    void DrawTestTriangle()
    {
        namespace wrl = Microsoft::WRL;

        struct Vertex
        {
            float x;
            float y;
        };

        auto size = 0.5f;
        const Vertex vertices[] =
        {
            { -size,size },
            { size,size },
            { -size, -size },
            // { 1.0f,-1.0f  },
            // { -1.0f,1.0f },
        };

        wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
        D3D11_BUFFER_DESC desc = {};
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.ByteWidth = sizeof(vertices);
        desc.StructureByteStride = sizeof(Vertex);
        
        D3D11_SUBRESOURCE_DATA resData ;
        resData.pSysMem = vertices;

        HRESULT hr;
        GFX_THROW_INFO(pDevice->CreateBuffer(&desc, &resData, &pVertexBuffer));

        // Bind vertex buffer to pipeline
        const UINT stride = sizeof(Vertex);
        const UINT offset = 0;
        pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset );

        // Create vertex shader
        wrl::ComPtr<ID3D11VertexShader> pVertexShader;
        wrl::ComPtr<ID3DBlob> pBlob;
        GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
        GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

        // Bind vertex shader
        pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
        
        // input (vertex) layout (2d position only)
        wrl::ComPtr<ID3D11InputLayout> pInputLayout;
        const D3D11_INPUT_ELEMENT_DESC ied[] = 
            {{"Position",
                0,
                DXGI_FORMAT_R32G32_FLOAT,
                0,
                0 ,
                D3D11_INPUT_PER_VERTEX_DATA,
                0
            }};

        
        GFX_THROW_INFO(pDevice->CreateInputLayout(
            ied,
            (UINT)std::size(ied),
            pBlob->GetBufferPointer(), //Must be the blob with the VertexShader
            pBlob->GetBufferSize(),
            &pInputLayout));

        // Bind Input Layer
        pContext->IASetInputLayout(pInputLayout.Get());

        // Create pixel shader
        wrl::ComPtr<ID3D11PixelShader> pPixelShared;
        GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
        GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShared));

        // Bind vertex shader
        pContext->PSSetShader(pPixelShared.Get(), nullptr, 0);

        // Bind Render Target
        pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), nullptr);

        //Set the primitive topology
        // Defines what to draw with the given points
        pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // Configure ViewPort
        D3D11_VIEWPORT vp {
            0,
            0,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            0,0
        };
        pContext->RSSetViewports(1, &vp);
        
        GFX_THROW_INFO_ONLY(pContext->Draw((UINT)std::size(vertices), 0))
    }


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

#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
};

