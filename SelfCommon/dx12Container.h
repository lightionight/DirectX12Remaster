#ifndef DX12CONTAINER_H
#define DX12CONTAINER_H

//Windows Include
#include <Windows.h>
#include <windowsx.h>
#include <wrl.h>

//DirectX 12 INCLUDE
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_4.h>

// d3d12 libraray
#pragma comment(lib, "d3d12.lib");
#pragma comment(lib, "dxgi.lib");

//std INClUDE

using Microsoft::WRL::ComPtr;

struct Dx12Instance
{
#if defined(DEBUG) || defined(_DEBUG)
    ComPtr<ID3D12Debug> DebugController = nullptr;
#endif
// BaseDevice
    ComPtr<IDXGIFactory4> DxgiFactory = nullptr;
    ComPtr<IDXGISwapChain> SwapChain = nullptr;
    ComPtr<ID3D12Device> D3dDevice = nullptr;

// Command
    ComPtr<ID3D12CommandQueue> CommandQueue = nullptr;
    ComPtr<ID3D12CommandAllocator> CommandAllocator = nullptr;
    ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr;

    // how many Render back buffer would you need
    static const int SwapChainBufferCount = 2;
    ComPtr<ID3D12Resource> SwapChainBuffer[SwapChainBufferCount]; // this is render targer buffer
    ComPtr<ID3D12Resource> DepthStencilBuffer = nullptr;

    ComPtr<ID3D12DescriptorHeap> RtvHeap = nullptr;
    ComPtr<ID3D12DescriptorHeap> DsvHeap = nullptr;

    // ViewPortSize
    D3D12_VIEWPORT ScreenViewPort;
    D3D12_RECT ScissorRect;

    DXGI_FORMAT BackBufferFormat;
    DXGI_FORMAT DepthStencilFormat;
};







#endif // !DX12CONTAINER_H
