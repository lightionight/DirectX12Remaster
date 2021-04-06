#ifndef DX12_CONTEXT_H
#define DX12_CONTEXT_H
////////////////////////////////////////////////////
/////          DirextX 12 Context        ///////////
////////////////////////////////////////////////////

// Include
#include <dx12book\d3dUtil.h>

// system include
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <iostream>
#include <d3d12sdklayers.h>

#include <dx12book\FrameResource.h>


//Self Include 
#include "d3dx12.h"
#include "Win32Context.h"

// Library Include
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib,"d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

//Forward Delcare
struct DxData;
struct DxDesc;
struct DxBind;
struct DxFeature;


struct DxFeature
{

};

struct DxData
{
public:
	ComPtr<IDXGIFactory4> DxgiFactory = nullptr;
	ComPtr<ID3D12Device> Device = nullptr;
	ComPtr<IDXGISwapChain> SwapChain = nullptr;
	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	ComPtr<ID3D12Fence> Fence = nullptr;
	UINT64 CurrentFence = 0;

	ComPtr<ID3D12CommandAllocator> CommandListAlloc = nullptr;
	ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr;
	ComPtr<ID3D12CommandQueue> CommandQueue = nullptr;

	static const int SwapChainBufferCount = 2;
	int CurrentBackBufferIndex = 0;
	ComPtr<ID3D12Resource> SwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> DepthStencilBuffer = nullptr;

	ComPtr<ID3D12DescriptorHeap> RtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> DsvHeap = nullptr;

	D3D12_VIEWPORT ScreenViewPort = { };
	D3D12_RECT ScissorRect = { };

	UINT RtvDescriptorSize = 0;
	UINT DsvDescriptorSize = 0;
	UINT CbvSrvUavDescriptorSize = 0;

#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> DebugController;
#endif
	DxData();
	void Initialize(const DxDesc*);

public:  // Parameter
	ID3D12Resource* CurrentBackBuffer() const 
	{ 
		return SwapChainBuffer[CurrentBackBufferIndex].Get(); 
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const 
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(RtvHeap->GetCPUDescriptorHandleForHeapStart(), CurrentBackBufferIndex, RtvDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStecilView() const
	{
		return DsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

public:
	void FlushCommandQueue();
	UINT CheckFeatureSupport(const DXGI_FORMAT&);
	void ResizeWindow(const DxDesc* , int , int);
	void EnableShaderBasedValidation();

	void PrepareRender(ID3D12PipelineState*, ComPtr<ID3D12CommandAllocator>);
	void AfterRender(FrameResource*);
};

struct DxDesc
{
	std::unique_ptr<DXGI_SWAP_CHAIN_DESC> SwapChainDesc = nullptr;
	std::unique_ptr<D3D12_COMMAND_QUEUE_DESC> QueueDesc = nullptr;
	std::unique_ptr<D3D12_DESCRIPTOR_HEAP_DESC> RtvHeapDesc = nullptr;
	std::unique_ptr<D3D12_DESCRIPTOR_HEAP_DESC> DsvHeapDesc = nullptr;
	D3D12_COMMAND_LIST_TYPE CommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Resize Desc
	std::unique_ptr<D3D12_RESOURCE_DESC> DepthStencilDesc = nullptr;
	std::unique_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> DsViewDesc = nullptr;
	std::unique_ptr<D3D12_CLEAR_VALUE> OptClear = nullptr;

	DxDesc();
	void Initialize(const WinDesc*, const DxData*, const WindowData*, bool, UINT);
	void ResizeDesc(int, int, bool, UINT);
};

struct DxBind
{
public:
	// Main
	ComPtr<ID3D12RootSignature>  RootSignnature = nullptr;
	ComPtr<ID3D12DescriptorHeap> SrvHeap = nullptr;
	static const int RootParameterNumber = 3;
	void Initialize(const DxData*);
	void AddRootParameter();
};



#endif // !DX12_CONTEXT_H