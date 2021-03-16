#include "SceneApp.h"

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

void SceneApp::BuildShadersAndInputLayout()
{
	mShaders["StandardVS"] = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

}


void SceneApp::BuildShapeGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
// Combine all geometry in one big buffer.
// 
// Calculate  VectexOffset 
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
// Calculate IndexOffset;
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();
// Box mesh Information
	SubmeshGeometry boxSubMesh;
	boxSubMesh.IndexCount = (UINT)box.Indices32.size();
	boxSubMesh.StartIndexLocation = boxIndexOffset;
	boxSubMesh.BaseVertexLocation = boxVertexOffset;

// Grid mesh Information
	SubmeshGeometry gridSubMesh;
	gridSubMesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubMesh.BaseVertexLocation = gridVertexOffset;
	gridSubMesh.StartIndexLocation = gridIndexOffset;

// Sphere mesh Information
	SubmeshGeometry sphereSubMesh;
	sphereSubMesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubMesh.BaseVertexLocation = sphereVertexOffset;
	sphereSubMesh.StartIndexLocation = sphereIndexOffset;
// Cylinder mesh Information
	SubmeshGeometry cylinderSubMesh;
	cylinderSubMesh.IndexCount = cylinder.Indices32.size();
	cylinderSubMesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubMesh.BaseVertexLocation = cylinderVertexOffset;

// Now Calculate All 
	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::DarkGreen);
	}
	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::BlueViolet);
	}
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
	}
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::Cyan);
	}
	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()),  std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

// Calculate Vertex array size and indices size;
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);
// Now Create geo pointer
	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "ShapeGeo";
//Create blob and copy data;
	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
// Upload buffer to GPU
	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(),
		vertices.data(),
		vbByteSize,
		geo->VertexBufferUploader);
	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(),
		indices.data(),
		ibByteSize,
		geo->IndexBufferUploader);
	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
// Sign index format that using GPU DATA FORMAT;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubMesh;
	geo->DrawArgs["grid"] = gridSubMesh;
	geo->DrawArgs["sphere"] = sphereSubMesh;
	geo->DrawArgs["cylinder"] = cylinderSubMesh;

	mGeometries[geo->Name] = std::move(geo);
}

void SceneApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
// All D3d Render Resource Pointer
	opaquePsoDesc.pRootSignature = mRootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
		mShaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
		mShaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = mDepthStencilFormat;
// after all set. using d3ddevice to instance;
	md3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframeDesc = opaquePsoDesc;
	opaqueWireframeDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	md3dDevice->CreateGraphicsPipelineState(&opaqueWireframeDesc, IID_PPV_ARGS(&mPSOs["wireframe"]));


}

void SceneApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(), 1, (UINT)mAllRItems.size()));
	}
}

void SceneApp::BuildRenderItems()
{
	auto boxRitem = std::make_unique<RenderItem>();
// Base On base Vertex using transpose matrix to offset item
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->objCBIndex = 0;
	boxRitem->geo = mGeometries["shapeGeo"].get();
	boxRitem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->indexCount = boxRitem->geo->DrawArgs["box"].IndexCount;
	boxRitem->startIndexLocation = boxRitem->geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->baseVertexLocation = boxRitem->geo->DrawArgs["box"].BaseVertexLocation;
	mAllRItems.push_back(std::move(boxRitem));

	auto gridItem = std::make_unique<RenderItem>();
	gridItem->World = MathHelper::Identity4x4();
	gridItem->objCBIndex = 1;
	gridItem->geo = mGeometries["shapeGeo"].get();
	gridItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridItem->indexCount = gridItem->geo->DrawArgs["grid"].IndexCount;
	gridItem->baseVertexLocation = gridItem->geo->DrawArgs["grid"].BaseVertexLocation;
	gridItem->startIndexLocation = gridItem->geo->DrawArgs["grid"].StartIndexLocation;
	mAllRItems.push_back(std::move(gridItem));

	for (auto& e : mAllRItems)
	{
		mOpaqueRitems.push_back(e.get());
	}
}
void SceneApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*> rItems)
{
	UINT objectByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	auto objectCB = mCurrentFrameResources->ObjectCB->Resource();

// Draw each Render Item
	for (size_t i = 0; i < rItems.size(); ++i)
	{
		auto ri = rItems[i];
		cmdList->IASetVertexBuffers(0, 1, &ri->geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->primitiveType);
// offset Constants buffer
		UINT cbvIndex = mCurrentFrameResourceIndex * (UINT)mOpaqueRitems.size() + ri->objCBIndex;
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(cbvIndex, mCbvSrvUavDescriptorSize);
		cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
		cmdList->DrawIndexedInstanced(ri->indexCount, 1, ri->startIndexLocation, ri->baseVertexLocation, 0);
	}
}