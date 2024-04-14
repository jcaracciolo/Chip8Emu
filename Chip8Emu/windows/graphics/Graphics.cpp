#include "Graphics.h"
#include <sstream>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    

    HRESULT hr;
    GFX_THROW_INFO(
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, // We require a hardware graphics card
                                  nullptr, swapCreateFlags, nullptr, // We don't care about featureLevel (like min-API supported)
                                  0, D3D11_SDK_VERSION, &sd, // Descriptor of what the swap chain and buffers should look like
                                  &pSwap, // SwapChain is in control of the front and back buffer
                                  &pDevice, // Device represents the graphics card and all the orchestration
                                  nullptr, // Output pointer to what feature level was achieved
                                  &pContext) // Context is in charge of rendering commands
    );

    wrl::ComPtr<ID3D11Resource> pBackBuffer;
    
    // gain access to the texture subresource in swap chain (back buffer)
    // 0 is the back buffer
    GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer));  // NOLINT(clang-diagnostic-language-extension-token)
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(),nullptr, &pTarget));
    
}

void Graphics::EndFrame()
{
    HRESULT hr;

#ifndef NDEBUG
    infoManager.Set();
#endif

    // Select 1 for hitting all the refresh frames, 2 can skip every other frame and such
    if( FAILED( hr = pSwap->Present(1u, 0) ) )
    {
        if( hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else
        {
            throw GFX_EXCEPT(hr);
        }
    }
}

void Graphics::ClearBuffer(float red, float green, float blue) const
{
    const float color[] = {red, green, blue};
    pContext->ClearRenderTargetView(pTarget.Get(), color);
    
}

Graphics::Exception::Exception(int line, const char* file, std::vector<std::string> infoMsgs) noexcept: Chip8Exception(line, file)
{
    // join all info messages with newlines into single string
    for( const auto& m : infoMsgs )
    {
        info += m;
        info.push_back( '\n' );
        info.push_back( '\n' );
    }
    // remove final newline if exists
    if( !info.empty() )
    {
        info.pop_back();
    }
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept: Exception(line, file, infoMsgs), hr(hr)
{
}

const char* Graphics::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << '\n'
    << "[ErrorInfo] \n" << GetErrorInfo() << '\n' << '\n'
    << GetOriginString();
    
    whatBuffer = oss.str();

    return whatBuffer.c_str();
}

const char* Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << '\n'
    << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec << " (" << GetErrorCode() << ")\n"
    << "[Description] \n" << GetErrorString() << '\n' << '\n'
    << "[ErrorInfo] \n" << GetErrorInfo() << '\n' << '\n'
    << GetOriginString();
    
    whatBuffer = oss.str();

    return whatBuffer.c_str();
}

const char* Graphics::Exception::GetType() const noexcept
{
    return "Graphics Exception";
}

const char* Graphics::HrException::GetType() const noexcept
{
    return "Graphics HResult Exception";
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

std::string Graphics::Exception::GetErrorInfo() const noexcept
{
    return info;
}

std::string Graphics::HrException::TranslateErrorCode(HRESULT hResult) noexcept
{
    // This only works on Windows8 and above, for windows 7 we must import the DxErr library
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

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    return TranslateErrorCode(hr);
}


