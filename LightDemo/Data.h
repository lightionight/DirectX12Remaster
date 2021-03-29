#pragma once
#include <d3dUtil.h>

using Microsoft::WRL::ComPtr;

struct WindowData
{
	HINSTANCE Hinstance = nullptr;
	HWND Hwnd = nullptr;
	LPCWSTR ClassName = nullptr;
	LPCWSTR WindowName = nullptr;
	int ClientWidth = 1980;
	int ClientHeight = 1080;
	WindowData(const LPCWSTR className, const  LPCWSTR windowName, int width, int height, WNDPROC wndProc)
	{
		ClassName = className;
		WindowName = windowName;
		Hinstance = GetModuleHandle(NULL);
		WNDCLASSEX wc = { NULL, CS_CLASSDC, wndProc, NULL, NULL, Hinstance, NULL, NULL, NULL, NULL, ClassName, NULL };
		RegisterClassEx(&wc);

		ClientHeight = width;
		ClientHeight = height;

		// Adjust Windows Size;
		RECT r = { 0 , 0 , ClientWidth, ClientHeight };
		AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
		int w = r.right - r.left;
		int h = r.bottom - r.top;

		Hwnd = CreateWindowEx(NULL, ClassName, WindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, Hinstance, NULL);
		ShowWindow(Hwnd, SW_SHOW);
		UpdateWindow(Hwnd);
	}
};

struct DirectData
{
public:
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<ID3D12Device> Device;
	ComPtr<IDXGISwapChain> SwapChain;
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;

	ComPtr<ID3D12Fence> Fence;
	UINT64 CurrentFence;

	ComPtr<ID3D12CommandAllocator> CommandListAlloc;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12CommandQueue> CommandQueue;

	static const int SwapChainBufferCount = 2;
	int CurrentBackBuffer = 0;
	DXGI_FORMAT BackBufferFormat;
	ComPtr<ID3D10Resource> SwapChainBuffer[SwapChainBufferCount];

	ComPtr<ID3D12Resource> RtvBuffer;
	ComPtr<ID3D12Resource> DsvBuffer;

	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12DescriptorHeap> DsvHeap;

	D3D12_VIEWPORT ScreenViewPort;
	D3D12_RECT ScissorRect;

	UINT RtvDescriptorSize = 0;
	UINT DsvDescriptorSize = 0;
	UINT CbvSrvUavDescriptorSize = 0;
#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> DebugController;
#endif

	DirectData(int width, int height, DXGI_SWAP_CHAIN_DESC* swapChainDesc)
	{
#if defined(DEBUG) || defined(_DEBUG)
		D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController));
		DebugController->EnableDebugLayer();
#endif
		CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory));
		if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device))))
		{

		}
		Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));

		SwapChainDesc = *swapChainDesc;

		// Create Resource
		RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		CbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		CreateCommandObject();
		CreateSwapChain();

	}
private:
	void CreateCommandObject()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = { };
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue));
		
		Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandListAlloc.GetAddressOf()));
		Device->CreateCommandList(NULL,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			CommandListAlloc.Get(),
			nullptr,
			IID_PPV_ARGS(CommandList.GetAddressOf()));
		CommandList->Close();
	}
	void CreateSwapChain()
	{
		SwapChain.Reset();
		DxgiFactory->CreateSwapChain(CommandQueue.Get(), &SwapChainDesc, IID_PPV_ARGS(SwapChain.GetAddressOf()));
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