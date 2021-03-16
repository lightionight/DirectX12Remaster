#include "SceneApp.h"

using Microsoft::WRL::ComPtr;

using namespace DirectX;


SceneApp::SceneApp(HINSTANCE hInstance) : D3DApp(hInstance) { }

SceneApp::~SceneApp()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

bool SceneApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;
// reset Commandlist
	mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr); 
// batch of step to Initialize d3d need REsource
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildShapeGeometry();
	BuildRenderItems();
	BuildFrameResources();
	BuildConstantBufferViews();
	BuildPSOs();
// Close CommandList And Excute Command
// CommandQueue is Manager of CommandList
	mCommandList->Close();
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
// Wait for Initialize Complete
	FlushCommandQueue();

	return true;
}

void SceneApp::OnResize()
{
// forward to base class deal with
	D3DApp::OnResize();
// update aspect ratio and recomputer projection matrix;
// This Place Get Some Comfuseings  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void SceneApp::Update(const GameTimer& gt)
{
	OnKeyBoardInput(gt);
	UpdateCamera(gt);
// Cycle FrameResource Array And Get Next FrameResource to store And Computer.
	mCurrentFrameResourceIndex = (mCurrentFrameResourceIndex + 1) % gNumFrameResources;
	mCurrentFrameResources = mFrameResources[mCurrentFrameResourceIndex].get();
	if (mCurrentFrameResources->Fence != 0 && mFence->GetCompletedValue() < mCurrentFrameResources->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		mFence->SetEventOnCompletion(mCurrentFrameResources->Fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
}

void SceneApp::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrentFrameResources->CmdListAlloc;
// Reset cmdListAlloc in FrameResource For Reuse.
	cmdListAlloc->Reset();
// base on isWireFrame to set difference pipeline state Ojbect
	if (isWireFrame)
	{
		mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get());
	}
	else
	{
		mCommandList->Reset(cmdListAlloc.Get(), mPSOs["Opaque"].Get());
	}
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

// indicate A state transition on resource usage. This operator base show for cpu gpu async    <----------
// Sounds Like Swap Bufffer, now is deal with backbuffer and send before backbuffer to render target.
	mCommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
		));
// Now Pointer to back buffer and before we Draw something we must clear it up and set all to initialized;
// Include RenderTargetBuffer and DepthStencilbuffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightBlue, 0, nullptr); 
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
// indicate buffer that we are going to draw
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
// ???????
	int passCbvIndex = mPassCbvOffset + mCurrentFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, mCbvSrvUavDescriptorSize);
	mCommandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);

// after all setting now you can draw something in the buffer forward to an other function to deal with draw object
	DrawRenderItems(mCommandList.Get(), mOpaqueRitems);

// after Draw Object, now you need re-indicate state transition on the resource usage.          <---------
	mCommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_PRESENT));

// after draw done. we need close mCommandlist for stop recording commands
	mCommandList->Close();
// Swap the front and back buffer.
	mSwapChain->Present(0, 0);
// Set backbuffer to next buffer index
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
// Set Fence + 1
	mCurrentFrameResources->Fence = ++mCurrentFence;  
// Set after done set new fence point in the timeline future.
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void SceneApp::OnMouseUp(WPARAM btnState, int x, int y)
{
// This function sounds like stop the rendering windows
	ReleaseCapture();
}

void SceneApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);
}

void SceneApp::OnMouseMove(WPARAM btnState, int x, int y)
{
// base on Mouse move to set viewSpace Change IFF When Left btn and right btn press down.
	if ((btnState & MK_LBUTTON) != 0)
	{
//  ?????????????? this sounds like calculate the distance from last mouse position.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		mTheta += dx;
		mPhi += dy;
// Restrict rotate view Angle
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
// make each pixel to 0.2unit in scene dx and dy is distance x and distance y
		float dx = 0.05f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.05f * static_cast<float>(y - mLastMousePos.y);
// update camera radius base on input.
		mRadius += dx - dy;
//Restrict the radius not move so far distance from target;
		mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void SceneApp::OnKeyBoardInput(const GameTimer& gt)
{
// this function is deal with keyboard input to show geometry wireframe or shader it
	if (GetAsyncKeyState('1') & 0x8000)
		isWireFrame = true;
	else
		isWireFrame = false;
}

void SceneApp::UpdateCamera(const GameTimer& gt)
{
// Convert spherical to cartesian coordinates
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);
// Using x y z to build view matrix.
	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

}

void SceneApp::UpdateObjectCBs(const GameTimer& gt)
{
	auto currObjectCB = mCurrentFrameResources->ObjectCB.get();
	for (auto& e : mAllRItems)
	{
// Only Update if Constants had change.
		if (e->NumFrameDirty > 0)
		{
// Load Constant buffer world matrix
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
// Update world matrix to current world matrix
			currObjectCB->CopyData(e->objCBIndex, objConstants);
			e->NumFrameDirty--;
		}
	}
}

void SceneApp::UpdateMainPassCB(const GameTimer& gt)
{
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
// Base on position matrix calculate inverse matrix
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();

// after all  mMainPass Constants updated
// get pass constants pointer in  FrameResource
	auto currentPassCB = mCurrentFrameResources->PassCB.get();  
	currentPassCB->CopyData(0, mMainPassCB);  //Copy data to framResources
}

/// <summary>
/// This function like build every Constants buffer view for each object for each frame resource.
/// </summary>
void SceneApp::BuildDescriptorHeaps()
{
	UINT objCount = (UINT)mOpaqueRitems.size();
	UINT numDescriptors = (objCount + 1) * gNumFrameResources;
// Save offset to PassCbv Offsets start position
	mPassCbvOffset = objCount * gNumFrameResources;
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
// Show all constants to shader let shader can using them for shader.
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
// Using d3d device to create heap, and let class member hold pointer of it
	md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap));
}

void SceneApp::BuildConstantBufferViews()
{
	UINT objByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	UINT objCount = (UINT)mOpaqueRitems.size();

	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
	{
		auto ojbectCB = mFrameResources[frameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < objCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = ojbectCB->GetGPUVirtualAddress();
// offset buffer address of each ojbect constant buffer
			cbAddress += i * objByteSize;
// meanwhile update object cbv in the descriptor heap, because there has gNumframeResource.
			int heapIndex = frameIndex * objCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objByteSize;
			md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
		}
	}
	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex)
	{
		auto passCB = mFrameResources[frameIndex]->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();  //<------------
		int heapIndex = mPassCbvOffset * frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;    //<--------------
		cbvDesc.SizeInBytes = passCBByteSize;
// Create constant buffer from md3ddevice.
		md3dDevice->CreateConstantBufferView(&cbvDesc, handle);

	}
}

void SceneApp::BuildRootSignature()
{
// first descritor table;
	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
// Second Descriptor table;
	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
// Create root parameter. the root parameter can be table or root descriptor or root constants
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

// Create root cbvs; now time start to gain heap memory and init them.
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		2, 
		slotRootParameter,
		0, 
		nullptr, 
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);
// Why there create blob???
	ComPtr<ID3DBlob> serializeRootSig = nullptr;
	ComPtr<ID3D10Blob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializeRootSig.GetAddressOf(),
		errorBlob.GetAddressOf()
	);

// feed parameter, and create root signatures;
	md3dDevice->CreateRootSignature(
		0,
		serializeRootSig->GetBufferPointer(),
		serializeRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())
	);
}

