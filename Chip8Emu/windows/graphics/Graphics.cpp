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

    SetupPipeline();
}

void Graphics::EndFrame()
{
    DrawPixels();
    
#ifndef NDEBUG
    infoManager.Set();
#endif

    HRESULT hr;
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

void Graphics::ClearBuffer(float red, float green, float blue)
{
    const float color[] = {red, green, blue};
    pContext->ClearRenderTargetView(pTarget.Get(), color);

    ClearScreen();
}

void Graphics::ClearScreen()
{
    indices.clear();
}

void Graphics::SetPixelOn(int x, int y)
{
    // We set up a Vertex Shader with all the possible values of the corners of the pixels
    // These coordinates go from 0 (left of first pixel) to CHIP8_WIDTH+1 (right of first pixel)
    // and form 0 (top of first pixel) to CHIP8_HEIGHT+1 (bottom of last pixel)
    UINT32 rowSize = CHIP8_WIDTH+1;
    
    const UINT32 topLeft = static_cast<UINT32>(x + y * rowSize);
    const UINT32 topRight = static_cast<UINT32>(topLeft + 1);
    const UINT32 bottomLeft = static_cast<UINT32>(topLeft + rowSize);
    const UINT32 bottomRight = static_cast<UINT32>(bottomLeft + 1);

    // We draw the triangles in a back facing way
    // (when using the left hand it will render the back of it
    // 2 triangles make 1 square
    indices.insert(indices.cend(), {bottomRight, bottomLeft, topLeft, topLeft, topRight, bottomRight});
}

void Graphics::DrawPixels()
{
    UINT size = static_cast<UINT>(std::size(indices));

    if(size == 0)
    {
        return ClearBuffer(0,0,0);
    }
    
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * size);
    ibd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA irsd = {};
    irsd.pSysMem = indices.data();
    HRESULT hr;
    GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &irsd, &pIndexBuffer));
    
    pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0 );
    
    GFX_THROW_INFO_ONLY(pContext->DrawIndexed(size,0, 0))
        
}

void Graphics::SetupPipeline()
{
    if(pipelineSetup)
    {
        throw GFX_CUSTOM_EXCEPT("Grtaphic Pipeline was already setup");
    }
    pipelineSetup = true;

    SetupVertexShader();
    SetupPixelShader();

    // Bind Render Target
    pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), nullptr);

    // Set the primitive topology
    // Defines what to draw with the given points
    pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Configure ViewPort
    D3D11_VIEWPORT vp {
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0,0
    };
    
    pContext->RSSetViewports(1, &vp);
}

void Graphics::SetupVertexShader()
{
    // Setup all possible corner of vertices for a CHIP8_WIDTH X CHIP8_HEIGHT screen to reference with indexes later
    Vertex vertices[CHIP8_HEIGHT+1][CHIP8_WIDTH+1];
    for(UINT16 y=0; y<=CHIP8_HEIGHT; y++) {
        for(UINT16 x=0; x<=CHIP8_WIDTH; x++) {
            vertices[y][x] = {x,y};
        }  
    }

    D3D11_BUFFER_DESC verBufferDesc = {};
    verBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    verBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    verBufferDesc.CPUAccessFlags = 0;
    verBufferDesc.MiscFlags = 0;
    verBufferDesc.ByteWidth = sizeof( vertices );
    verBufferDesc.StructureByteStride = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA resData = {};
    resData.pSysMem = vertices;

    HRESULT hr;
    GFX_THROW_INFO(pDevice->CreateBuffer(&verBufferDesc, &resData, &pVertexBuffer));
    
    // Bind vertex buffer to pipeline
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset );

    // create vertex shader
    GFX_THROW_INFO( D3DReadFileToBlob( L"Chip8VertexShader.cso",&pBlob ) );
    GFX_THROW_INFO( pDevice->CreateVertexShader( pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr, &pVertexShader ) );
    
    // bind vertex shader
    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

    // input (vertex) layout (2d position only)
    const D3D11_INPUT_ELEMENT_DESC ied[] = 
        {{"Position",
            0,
            DXGI_FORMAT_R16G16_UINT,
            0,
            0 ,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }};
    
    // Create the Input Layout to tell what type of data we are sending to the VertexShader
    GFX_THROW_INFO(pDevice->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), //Must be the blob with the VertexShader
        pBlob->GetBufferSize(),
        &pInputLayout));

    // Bind Input Layer
    pContext->IASetInputLayout(pInputLayout.Get());
}

void Graphics::SetupPixelShader()
{
    HRESULT hr;

    // Create Pixel Shader
    GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

    // Bind Pixel Shader
    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);
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



