#include "TerrainApp.h"

TerrainApp::TerrainApp(HINSTANCE hInstace) : D3DApp(hInstace) { }
TerrainApp::~TerrainApp() { }
bool TerrainApp::Initialize()
{
// forword to base class deal with
	D3DApp::Initialize();
	mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);

	BuildRootSignature();
	BuildShaderAndInputLayout();
	BuildGeometry();
	BuildGeometryBuffer();
	BuildRenderItems();
	BuildFrameResource();
	BuildPSOs();

	mCommandList->Close();
	ID3D12CommandList* cmdList[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

	FlushCommandQueue();
	return true;
}
// if window resize, need update viewport aspect Ratio and projection matrix
void TerrainApp::OnResize()
{
	D3DApp::OnResize();
	XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25 * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
// Update project matrix
	XMStoreFloat4x4(&mProj, proj);
}
void TerrainApp::Update(const GameTimer& gt)
{
	OnkeyBoardInput(gt);
	UpdateCameraPosition(gt);
//cycle frame resourece
	mCurrentFrameResourceIndex = (mCurrentFrameResourceIndex + 1) % gNumFrameResources;
	mCurrentFrameResource = mFrameResources[mCurrentFrameResourceIndex].get();
	if (mCurrentFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrentFrameResource->Fence)
	{
		HANDLE handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		mFence->SetEventOnCompletion(mCurrentFrameResource->Fence, handle);
		WaitForSingleObject(handle, INFINITE);
		CloseHandle(handle);
	}

	UpdateObjectsCB(gt);
	UpdateMainPassCB(gt);
    // UpdateWave(gt);
}
void TerrainApp::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrentFrameResource->CmdListAlloc;
	cmdListAlloc->Reset();
	if (_IsWireFrame)
		mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get());
	else
		mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get());
//Reset viewport
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
// indicate state transition for resource usage let cpu and gpu know 
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET));
// Clear Render buffer and depth buffer
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::Blue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
// Bind per-pass constant buffer
	auto passCB = mCurrentFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());
// Now we can Draw our current pass all object
	DrawRenderItems(mCommandList.Get(), mRItemLayer[(int)RenderLayer::Opaque]);
// after all command set
// indicate resource state transition for resource usage let cpu and gpu both know that
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT));
// after done before excute need close.
	mCommandList->Close();
	ID3D12CommandList* cmdlist[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdlist), cmdlist);
// after done excute 
	mSwapChain->Present(0, 0);
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
	//add fence
	mCurrentFrameResource->Fence = ++mCurrentFence;
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}
void TerrainApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mMousePos.x = x;
	mMousePos.y = y;
	SetCapture(mhMainWnd);  // what about this function?
}
void TerrainApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void TerrainApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
// Calculate mouse move distance
		float dx = XMConvertToRadians()
	}
}