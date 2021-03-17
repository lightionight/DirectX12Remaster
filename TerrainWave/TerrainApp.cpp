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
	auto cmdListAlloc = mCurrentFrameResource->
}