#pragma once

#include <d3dUtil.h>
#include <iostream>

using Microsoft::WRL::ComPtr;

struct WindowData
{
	HINSTANCE Hinstance = nullptr;
	HWND Hwnd = nullptr;
	LPCWSTR ClassName = nullptr;
	LPCWSTR WindowName = nullptr;
	int ClientWidth = 1980;
	int ClientHeight = 1080;
	void Initialize(const LPCWSTR className, const  LPCWSTR windowName, int width, int height, WNDPROC wndProc)
	{
		ClassName = className;
		WindowName = windowName;
		Hinstance = GetModuleHandle(NULL);
		if (Hinstance == nullptr)
			std::cout << "HInstace Get ERROr";
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, wndProc, NULL, NULL, Hinstance, NULL, NULL, NULL, NULL, ClassName, NULL };
		RegisterClassEx(&wc);

		ClientHeight = width;
		ClientHeight = height;

		// Adjust Windows Size;
		RECT r = { 0 , 0 , ClientWidth, ClientHeight };
		::AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
		int w = r.right - r.left;
		int h = r.bottom - r.top;

		Hwnd = ::CreateWindow(ClassName, WindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, Hinstance, NULL);
		
		::ShowWindow(Hwnd, SW_SHOWDEFAULT);

		UpdateWindow(Hwnd);
	}
};

struct DirectDesc
{
	DXGI_SWAP_CHAIN_DESC* SwapChainDesc = nullptr;
	D3D12_COMMAND_QUEUE_DESC* QueueDesc = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC* RtvHeapDesc = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC* DsvHeapDesc = nullptr;
	D3D12_COMMAND_LIST_TYPE CommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Resize Desc
	D3D12_RESOURCE_DESC* DepthStencilDesc = nullptr;
	D3D12_DEPTH_STENCIL_VIEW_DESC* DsViewDesc = nullptr;
	D3D12_CLEAR_VALUE* OptClear = nullptr;


	void Initialize()
	{
		InitSwapChainDesc();
		InitCommandQueue();
		InitDescriptorHeapDesc();
	}

	void ResizeDesc(int clientWidth, int clientHeight, bool msaaState, UINT msaaQuality)
	{
		D3D12_RESOURCE_DESC DSDesc;
		DSDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		DSDesc.Alignment = 0;
		DSDesc.Width = clientWidth;
		DSDesc.Height = clientHeight;
		DSDesc.DepthOrArraySize = 1;
		DSDesc.MipLevels = 1;
		DSDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		DSDesc.SampleDesc.Count = msaaState ? 4 : 1;
		DSDesc.SampleDesc.Quality = msaaState ? (msaaQuality - 1) : 0;
		DSDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		DSDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		DepthStencilDesc = &DSDesc;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DepthStencilFormat;
		optClear.DepthStencil.Stencil = 0;
		optClear.DepthStencil.Depth = 1.0f;
		OptClear = &optClear;

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		DsViewDesc = &dsvDesc;

	}
private:
	void InitSwapChainDesc()
	{
		DXGI_SWAP_CHAIN_DESC dscd;
		dscd.BufferDesc.RefreshRate.Numerator = 60;
		dscd.BufferDesc.RefreshRate.Denominator = 1;
		dscd.BufferDesc.Format = BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dscd.Windowed = true;
		dscd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		SwapChainDesc = &dscd;
	}
	void InitCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = CommandListType;
		QueueDesc = &queueDesc;
	}
	void InitDescriptorHeapDesc()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NodeMask = 0;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		RtvHeapDesc = &rtvHeapDesc;

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapdesc;
		dsvHeapdesc.NumDescriptors = 1;
		dsvHeapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapdesc.NodeMask = 0;
		DsvHeapDesc = &dsvHeapdesc;
	}

};

struct WindowStatu
{
	bool Paused = false;
	bool Minimized = false;
	bool Maximized = false;
	bool Resizing = false;
	bool FullScreen = false;
};

struct DirectData
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

	void Initialize(const DirectDesc& desc)
	{
#if defined(DEBUG) || defined(_DEBUG)
		D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController));
		DebugController->EnableDebugLayer();
#endif
		CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory));

		D3D12CreateDevice(nullptr, FeatureLevel, IID_PPV_ARGS(&Device));

		Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
		// Create Resource
		RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		CbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		Device->CreateCommandQueue(desc.QueueDesc, IID_PPV_ARGS(&CommandQueue));

		Device->CreateCommandAllocator(desc.CommandListType, IID_PPV_ARGS(CommandListAlloc.GetAddressOf()));

		Device->CreateCommandList(NULL, desc.CommandListType, CommandListAlloc.Get(), nullptr,
			                      IID_PPV_ARGS(CommandList.GetAddressOf()));
		CommandList->Close();
		
		SwapChain.Reset();

		if (FAILED(DxgiFactory->CreateSwapChain(CommandQueue.Get(), desc.SwapChainDesc, SwapChain.GetAddressOf())))
			std::cout << "Create SwapChain Error" << std::endl;

		Device->CreateDescriptorHeap(desc.RtvHeapDesc, IID_PPV_ARGS(RtvHeap.GetAddressOf()));

		Device->CreateDescriptorHeap(desc.DsvHeapDesc, IID_PPV_ARGS(DsvHeap.GetAddressOf()));

	}
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
	void ResizeWindow(const DirectDesc& desc, int clientWidth, int clientHeight)
	{
		assert(Device);
		assert(SwapChain);
		assert(CommandListAlloc);

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
			desc.DepthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			desc.OptClear,
			IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));
		Device->CreateDepthStencilView(
			DepthStencilBuffer.Get(),
			desc.DsViewDesc,
			DepthStecilView());
		
		CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		CommandList->Close();
		ID3D12CommandList* cmdList[] = { CommandList.Get() };
		CommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

		FlushCommandQueue();

		ScreenViewPort.TopLeftX = 0;
		ScreenViewPort.TopLeftY = 0;
		ScreenViewPort.Width = static_cast<float>(desc.DepthStencilDesc->Width);
		ScreenViewPort.Height = static_cast<float>(desc.DepthStencilDesc->Height);
		ScreenViewPort.MinDepth = 0.0f;
		ScreenViewPort.MaxDepth = 1.0f;

		ScissorRect = { 0 , 0, (long)ScreenViewPort.Width, (long)ScreenViewPort.Height };
	}

	
};

