#include "Dx12Context.h"
#include <d3dx12.h>
#include <DirectXColors.h>
#include <Self\SceneManager.h>


DxDesc::DxDesc()
{
	SwapChainDesc = std::make_unique<DXGI_SWAP_CHAIN_DESC>();
	QueueDesc = std::make_unique<D3D12_COMMAND_QUEUE_DESC>();
	RtvHeapDesc = std::make_unique<D3D12_DESCRIPTOR_HEAP_DESC>();
	DsvHeapDesc = std::make_unique<D3D12_DESCRIPTOR_HEAP_DESC>();
	DepthStencilDesc = std::make_unique<D3D12_RESOURCE_DESC>();
	DsViewDesc = std::make_unique<D3D12_DEPTH_STENCIL_VIEW_DESC>();
	OptClear = std::make_unique<D3D12_CLEAR_VALUE>();
}

void DxDesc::Initialize(const WinDesc* winDesc, const DxData* directData, const WindowData* winData, bool msaaState, UINT msaaQuality)
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

void DxDesc::ResizeDesc(int clientWidth, int clientHeight, bool msaaState, UINT msaaQuality)
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


DxData::DxData()
{
#if defined(DEBUG) || defined(_DEBUG)
    D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController));
	DebugController->QueryInterface(IID_PPV_ARGS(&DebugController1));
	DebugController1->SetEnableGPUBasedValidation(true);
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

void DxData::Initialize(const DxDesc* desc)
{

	Device->CreateCommandQueue(desc->QueueDesc.get(), IID_PPV_ARGS(&CommandQueue));

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

void DxData::FlushCommandQueue()
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

UINT DxData::CheckFeatureSupport(const DXGI_FORMAT& backBufferFormat)
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

void DxData::ResizeWindow(const DxDesc* desc, int clientWidth, int clientHeight)
{
	assert(Device);
	assert(SwapChain);
	assert(CommandListAlloc);

	FlushCommandQueue();

	ThrowIfFailed(CommandList->Reset(CommandListAlloc.Get(), nullptr));

	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		SwapChainBuffer[i].Reset();
	}

	DepthStencilBuffer.Reset();

	ThrowIfFailed(SwapChain->ResizeBuffers(
		SwapChainBufferCount,
		clientWidth,
		clientHeight,
		desc->BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	CurrentBackBufferIndex = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i]));
		Device->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, RtvDescriptorSize);
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

void DxData::PrepareRender(ID3D12PipelineState* pso , ComPtr<ID3D12CommandAllocator> cmdListAlloc)
{
	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(CommandList->Reset(cmdListAlloc.Get(), pso));

	CommandList->RSSetScissorRects(1, &ScissorRect);
	CommandList->RSSetViewports(1, &ScreenViewPort);

	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	CommandList->ClearDepthStencilView(DepthStecilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStecilView());
}

void DxData::AfterRender(FrameResource* framesource)
{
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* cmdlist[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(cmdlist), cmdlist);

	ThrowIfFailed(SwapChain->Present(0, 0));

	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % SwapChainBufferCount;

	framesource->Fence = ++CurrentFence;

	CommandQueue->Signal(Fence.Get(), CurrentFence);
}

void DxData::InitCommand(ComPtr<ID3D12CommandAllocator> cmdListAlloc)
{
	if (cmdListAlloc)
		CommandList->Reset(cmdListAlloc.Get(), nullptr);
	else
		CommandList->Reset(CommandListAlloc.Get(), nullptr);
}

void DxData::CommandExcute()
{
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* cmdList[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

	FlushCommandQueue();
}


void DxData::EnableShaderBasedValidation()
{
	
}

void DxBind::Initialize(const DxData* dxData)
{
	InitSamples();

	CD3DX12_ROOT_PARAMETER slotRootParameter[RootParameterNumber];
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsConstantBufferView(2);
	slotRootParameter[3].InitAsConstantBufferView(3);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter, (UINT)TexSamples.size(), TexSamples.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	dxData->Device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(RootSignnature.GetAddressOf()));

	InitSrvHeap(4, dxData->Device.Get());

}


void DxBind::InitSamples()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRigister
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp
	(
		1,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap
	(
		2,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp
	(
		3,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap
	(
		4,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp
	(
		5,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	TexSamples.push_back(pointWrap);
	TexSamples.push_back(pointClamp);
	TexSamples.push_back(linearWrap);
	TexSamples.push_back(linearClamp);
	TexSamples.push_back(anisotropicWrap);
	TexSamples.push_back(anisotropicClamp);
}

void DxBind::InitSrvHeap(UINT number, ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = { };
	MaxTexSrvNum = srvHeapDesc.NumDescriptors = number;
	
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&SrvHeap)));
}

void DxBind::AddSrvDescToSrvHeap(ID3D12Device* device, SceneManager* sceneManager, const std::string& texName)
{
	if((UINT)TexSrvCount > MaxTexSrvNum)
	{
		std::cout << "Error for add New Texture Resource. " << std::endl;
	}
	
	// Get CPU Heap Address
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(SrvHeap->GetCPUDescriptorHandleForHeapStart());

	
	auto tex = sceneManager->GetTexturePointer(texName)->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = { };
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = tex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = tex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    //Need Offset, if there is not only one Texture;
	hDescriptor.Offset(TexSrvCount, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));

	device->CreateShaderResourceView(tex.Get(),&srvDesc, hDescriptor);

	++TexSrvCount;
}


void DxBind::AddRootParameter()
{

}
