#ifndef UI_HPP
#define UI_HPP

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};

class ui {
public:
    bool init();
    bool update();
    ~ui();
    void imgui_windows();


public:
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void WaitForLastSubmittedFrame();
    FrameContext* WaitForNextFrameResources();
    bool running() { return !done; }
    // Our state
    bool done = false;
    // bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    WNDCLASSEXW wc;
    HWND hwnd;

public:
    ID3D12Device*                g_pd3dDevice = NULL;
    IDXGISwapChain3*             g_pSwapChain = NULL;

private:
    // Data
    static const int NUM_FRAMES_IN_FLIGHT = 3;
    static const int NUM_BACK_BUFFERS = 3;
    FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
    UINT                         g_frameIndex = 0;

    
    ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL;
    ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL;
    ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;
    ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL;
    ID3D12Fence*                 g_fence = NULL;
    HANDLE                       g_fenceEvent = NULL;
    UINT64                       g_fenceLastSignaledValue = 0;
    HANDLE                       g_hSwapChainWaitableObject = NULL;
    ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
    D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
};



#endif
