#ifndef DX12_CONTEXT_H
#define DX12_CONTEXT_H
////////////////////////////////////////////////////
/////          DirextX 12 Context        ///////////
////////////////////////////////////////////////////

// Include
#include <dx12book\d3dUtil.h>

// system include
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iostream>


//Self Include 
#include "d3dx12.h"
#include "Win32Context.h"

// Library Include
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

struct DirectFeature
{

};

typedef struct DxData
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

	std::string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}


#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> DebugController;
#endif
	DxData()
	{
#if defined(DEBUG) || defined(_DEBUG)
		D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController));
		DebugController->EnableDebugLayer();
#endif
		CreateDXGIFactory2(NULL, IID_PPV_ARGS(&DxgiFactory));

		D3D12CreateDevice(nullptr, FeatureLevel, IID_PPV_ARGS(&Device));

		Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));

		// Create Resource
		RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		CbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Initialize(const DxDesc* desc)
	{
		ThrowIfFailed(Device->CreateCommandQueue(desc->QueueDesc.get(), IID_PPV_ARGS(&CommandQueue)));

		if (FAILED(Device->CreateCommandAllocator(desc->CommandListType, IID_PPV_ARGS(CommandListAlloc.GetAddressOf()))))
			std::cout << "Create Command Allocator Error in Data.h " << std::endl;

		Device->CreateCommandList(NULL, desc->CommandListType, CommandListAlloc.Get(), nullptr,
			IID_PPV_ARGS(CommandList.GetAddressOf()));
		CommandList->Close();

		SwapChain.Reset();
		if (FAILED(DxgiFactory->CreateSwapChain(CommandQueue.Get(), desc->SwapChainDesc.get(), SwapChain.GetAddressOf())))
			std::cout << "Create SwapChain Error" << std::endl;

		Device->CreateDescriptorHeap(desc->RtvHeapDesc.get(), IID_PPV_ARGS(RtvHeap.GetAddressOf()));
		Device->CreateDescriptorHeap(desc->DsvHeapDesc.get(), IID_PPV_ARGS(DsvHeap.GetAddressOf()));
	}
public:  // Parameter
	ID3D12Resource* CurrentBackBuffer()const
	{
		return SwapChainBuffer[CurrentBackBufferIndex].Get();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			RtvHeap->GetCPUDescriptorHandleForHeapStart(),
			CurrentBackBufferIndex,
			RtvDescriptorSize);
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStecilView() const
	{
		return DsvHeap->GetCPUDescriptorHandleForHeapStart();
	}
public:
	void FlushCommandQueue()
	{
		CurrentFence++;
		CommandQueue->Signal(Fence.Get(), CurrentFence);
		if (Fence->GetCompletedValue() < CurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			Fence->SetEventOnCompletion(CurrentFence, eventHandle);

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}
	UINT CheckFeatureSupport(const DXGI_FORMAT& backBufferFormat)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
		qualityLevels.Format = backBufferFormat;
		qualityLevels.SampleCount = 4;
		qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		qualityLevels.NumQualityLevels = 0;
		Device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&qualityLevels,
			sizeof(qualityLevels));
		return qualityLevels.NumQualityLevels;
	}
	void ResizeWindow(const DxDesc* desc, int clientWidth, int clientHeight)
	{
		//assert(Device);
		//assert(SwapChain);
		//assert(CommandListAlloc);

		FlushCommandQueue();

		CommandList->Reset(CommandListAlloc.Get(), nullptr);

		for (int i = 0; i < SwapChainBufferCount; ++i)
			SwapChainBuffer[i].Reset();
		DepthStencilBuffer.Reset();

		CurrentBackBufferIndex = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < SwapChainBufferCount; ++i)
		{
			SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i]));
			Device->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		}

		Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			desc->DepthStencilDesc.get(),
			D3D12_RESOURCE_STATE_COMMON,
			desc->OptClear.get(),
			IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));
		Device->CreateDepthStencilView(
			DepthStencilBuffer.Get(),
			desc->DsViewDesc.get(),
			DepthStecilView());

		CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		CommandList->Close();
		ID3D12CommandList* cmdList[] = { CommandList.Get() };
		CommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

		FlushCommandQueue();

		ScreenViewPort.TopLeftX = 0;
		ScreenViewPort.TopLeftY = 0;
		ScreenViewPort.Width = static_cast<float>(desc->DepthStencilDesc->Width);
		ScreenViewPort.Height = static_cast<float>(desc->DepthStencilDesc->Height);
		ScreenViewPort.MinDepth = 0.0f;
		ScreenViewPort.MaxDepth = 1.0f;

		ScissorRect = { 0 , 0, (long)ScreenViewPort.Width, (long)ScreenViewPort.Height };
	}
}DxData;

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

	DxDesc()
	{
		SwapChainDesc = std::make_unique<DXGI_SWAP_CHAIN_DESC>();
		QueueDesc = std::make_unique<D3D12_COMMAND_QUEUE_DESC>();
		RtvHeapDesc = std::make_unique<D3D12_DESCRIPTOR_HEAP_DESC>();
		DsvHeapDesc = std::make_unique<D3D12_DESCRIPTOR_HEAP_DESC>();
		DepthStencilDesc = std::make_unique<D3D12_RESOURCE_DESC>();
		DsViewDesc = std::make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>();
		OptClear = std::make_unique<D3D12_CLEAR_VALUE>();
	}

	void Initialize(const WinDesc* winDesc, const DxData* directData, const WindowData* winData, bool msaaState, UINT msaaQuality
	)
	{
		// Swap Chain Desc
		SwapChainDesc->BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc->BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc->BufferDesc.Format = BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		SwapChainDesc->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		SwapChainDesc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc->Windowed = true;
		SwapChainDesc->SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		SwapChainDesc->BufferDesc.Width = winDesc->ClientWidth;
		SwapChainDesc->BufferDesc.Height = winDesc->ClientHeight;
		SwapChainDesc->SampleDesc.Count = msaaState ? 4 : 1;
		SwapChainDesc->SampleDesc.Quality = msaaState ? (msaaQuality - 1) : 0;
		SwapChainDesc->BufferCount = directData->SwapChainBufferCount;
		SwapChainDesc->OutputWindow = winData->Hwnd;

		// Command Queue Desc
		QueueDesc->Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueueDesc->Type = CommandListType;
		QueueDesc->Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

		//RTV 
		RtvHeapDesc->Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RtvHeapDesc->NodeMask = 0;
		RtvHeapDesc->Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		RtvHeapDesc->NumDescriptors = directData->SwapChainBufferCount;

		// DSV
		DsvHeapDesc->Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		DsvHeapDesc->NodeMask = 0;
		DsvHeapDesc->Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DsvHeapDesc->NumDescriptors = 1;

		//Depth Stencil View
		DsViewDesc->Flags = D3D12_DSV_FLAG_NONE;
		DsViewDesc->ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DsViewDesc->Format = DepthStencilFormat;
		DsViewDesc->Texture2D.MipSlice = 0;

		// Clear value Desc
		OptClear->Format = DepthStencilFormat;
		OptClear->DepthStencil.Stencil = 0;
		OptClear->DepthStencil.Depth = 1.0f;

	}

	void ResizeDesc(int clientWidth, int clientHeight, bool msaaState, UINT msaaQuality)
	{

		DepthStencilDesc->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		DepthStencilDesc->Alignment = 0;
		DepthStencilDesc->Width = clientWidth;
		DepthStencilDesc->Height = clientHeight;
		DepthStencilDesc->DepthOrArraySize = 1;
		DepthStencilDesc->MipLevels = 1;
		DepthStencilDesc->Format = DXGI_FORMAT_R24G8_TYPELESS;
		DepthStencilDesc->SampleDesc.Count = msaaState ? 4 : 1;
		DepthStencilDesc->SampleDesc.Quality = msaaState ? (msaaQuality - 1) : 0;
		DepthStencilDesc->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		DepthStencilDesc->Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	}
};





struct DxBind
{
public:
	// Main
	ComPtr<ID3D12RootSignature>  RootSignnature = nullptr;
	ComPtr<ID3D12DescriptorHeap> SrvHeap = nullptr;
	static const int RootParameterNumber = 3;
	void Initialize(const DxData* dxData)
	{
		CD3DX12_ROOT_PARAMETER slotRootParameter[RootParameterNumber];
		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsConstantBufferView(2);

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;

		D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());

		dxData->Device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(), IID_PPV_ARGS(RootSignnature.GetAddressOf()));

	}
	void AddRootParameter()
	{

	}
};



#endif // !DX12_CONTEXT_H