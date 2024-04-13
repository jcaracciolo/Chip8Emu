#pragma once
#include <string>
#include <vector>
#include <dxgidebug.h>
#include <wrl.h>
#include "../LightWindows.h"

class DxgiInfoManager
{
public:
    DxgiInfoManager();
    ~DxgiInfoManager() = default;
    DxgiInfoManager(const DxgiInfoManager& other) = delete;
    DxgiInfoManager(DxgiInfoManager&& other) noexcept = delete;
    DxgiInfoManager& operator=(const DxgiInfoManager& other) = delete;
    DxgiInfoManager& operator=(DxgiInfoManager&& other) noexcept = delete;


    //API
    void Set() noexcept;
    std::vector<std::string> GetMessages() const;

private:
    unsigned long long next = 0u;
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};
