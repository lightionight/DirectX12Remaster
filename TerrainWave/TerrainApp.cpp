#include "TerrainApp.h"

TerrainApp::TerrainApp(HINSTANCE hInstace) : D3DApp(hInstace) { }

TerrainApp::~TerrainApp() { }

//---------------------------------------------Initialize Funciton----------------------------------------------------//
bool TerrainApp::Initialize()
{
	
	// forword to base class deal with
	if (!D3DApp::Initialize())
		return false;
	mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);
	mWaves = std::make_unique<Wave>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
	mSceneManager = std::make_unique<SceneManager>();
	BuildRootSignature();
	BuildShaderAndInputLayout();
	BuildPSOs();
	BuildLandGeometry();
	BuildWaterGeometry();
	//BuildRenderItems();
	BuildRenderItems();
	BuildFrameResource();
	mCommandList->Close();
	ID3D12CommandList* cmdList[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);
	FlushCommandQueue();
	return true;
}
// if window resize, need update viewport aspect Ratio and projection matrix
void TerrainApp::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];
	// init as CBV
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2,
		slotRootParameter,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	// serialize root signature
	D3D12SerializeRootSignature(&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(),
		errorBlob.GetAddressOf());
	// using device to create it.
	md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf()));
}

void TerrainApp::BuildShaderAndInputLayout()
{
	// Using Path Create Shader
	mSceneManager->AddShader("Default", L"Shaders\\color.hlsl", L"Shaders\\color.hlsl", std::wstring());
	mInputLayout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } };
}

void TerrainApp::BuildLandGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);
	std::vector<Vertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		// Transfer Data from geoGem to Current function variable
		auto& p = grid.Vertices[i].Position;
		vertices[i].Pos = p;
		//from wave reset point height y
		vertices[i].Pos.y = GetHillsHeight(p.x, p.z);
		// Reference height to set vertice color
		float currHeight = vertices[i].Pos.y;
		if (currHeight < -10.0f)
			vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		else if (currHeight < 5.0f)
			vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		else if (currHeight < 12.0f)
			vertices[i].Color = XMFLOAT4(0.1f, 0.2f, 0.35f, 1.0f);
		else if (currHeight < 20.0f)
			vertices[i].Color = XMFLOAT4(0.45f, 0.2f, 0.34f, 1.0f);
		else
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	std::vector<std::uint16_t> indices = grid.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mSceneManager->AddGeo(md3dDevice, mCommandList, "LandGeo", "grid", vbByteSize, ibByteSize, vertices, indices);
}

void TerrainApp::BuildWaterGeometry()
{
	std::vector<std::uint16_t> indices(3 * mWaves->TriangleCount()); // a triangle need 3 indices
	int m = mWaves->RowCount();
	int n = mWaves->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;

		}
	}
	UINT vbByteSize = mWaves->vertexCount() * sizeof(Vertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mSceneManager->AddGeo(md3dDevice, mCommandList, "WaterGeo", "grid", vbByteSize, ibByteSize, indices);
}

void TerrainApp::BuildPSOs()
{
	mSceneManager->AddPso(L"Default",
		md3dDevice,
		mRootSignature,
		mInputLayout,
		mSceneManager->UseShader("Default"),
		mBackBufferFormat,
		mDepthStencilFormat,
		m4xMsaaState,
		m4xMsaaQuality);
}

void TerrainApp::BuildFrameResource()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),
			1, (*(mSceneManager->AllRenderItem())).size(), mWaves->vertexCount()));
	};
}

void TerrainApp::BuildRenderItems()
{
	// Water
	auto wavesRitem = std::make_unique<RenderItem>();
	wavesRitem->World = MathHelper::Identity4x4();
	wavesRitem->ObjIndex = 0;
	wavesRitem->Geo = mSceneManager->GetGeoPointer("WaterGeo");
	wavesRitem->Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	// Give pointer to another
	mWavesRitems = wavesRitem.get();
	// Add to Scene All item
	mSceneManager->AddToSceneitems(&(wavesRitem));
	// Addto RenderLayer Item
	mSceneManager->AddToRenderLayer(RenderLayer::Opaque, wavesRitem.get());

	// Land
	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = MathHelper::Identity4x4();
	gridRitem->ObjIndex = 1;
	gridRitem->Geo = mSceneManager->GetGeoPointer("LandGeo");
	gridRitem->Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	// Add to scene item
	mSceneManager->AddToSceneitems(&(gridRitem));

	// Add to Render Layer
	mSceneManager->AddToRenderLayer(RenderLayer::Opaque, gridRitem.get());


}

//---------------------------------------------Update Function--------------------------------------------------------//
void TerrainApp::OnResize()
{
	D3DApp::OnResize();
	XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	// Update project matrix
	XMStoreFloat4x4(&mProj, proj);
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
	if ((btnState & MK_LBUTTON) != 0) // rotate
	{
		// Calculate mouse move distance
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mMousePos.y));
		// Using Calculate radians to add
		mTheta += dx;
		mPhi += dy;
		// add Restrict the angle
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState && MK_RBUTTON) != 0) // Scale
	{
		float dx = 0.2f * static_cast<float>(x - mMousePos.x);
		float dy = 0.2f * static_cast<float>(y - mMousePos.y);
		// Then using dx - dy to get move distance and update radius R
		mRadius += dx - dy;
		// Restrict scale value, keep camera not move so far or so close
		mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
	}
	mMousePos.x = x;
	mMousePos.y = y;
}

void TerrainApp::OnkeyBoardInput(const GameTimer& gt)
{
	if (GetAsyncKeyState('1') & 0x8000)
		_IsWireFrame = true;
	if (GetAsyncKeyState('2') & 0x8000)
		_IsWireFrame = false;
}

void TerrainApp::UpdateCameraPosition(const GameTimer& gt)
{
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);
	XMVECTOR pos = DirectX::XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = DirectX::XMVectorZero();
	XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void TerrainApp::UpdateObjectsCB(const GameTimer& gt)
{
	auto currentObjectCB = mCurrentFrameResource->ObjectCB.get();
	for (auto& e : *(mSceneManager->AllRenderItem()))
	{
		if (e->NumFrameDirty > 0)
		{
			// beacuse every object has them own offset position, so every object has own world matrix
			// World matrix is ojbect Constants
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			// and copy object Constant buffer to current for render
			ObjectConstants objectConstants;
			XMStoreFloat4x4(&objectConstants.World, XMMatrixTranspose(world));
			currentObjectCB->CopyData(e->ObjIndex, objectConstants);
			// NumFramesDirty like sider Index to point out n frameResource need update or not.
			e->NumFrameDirty--;
		}
	}
}

// this function using input parameter timer maybe will using future for animation, for now it's usless.
void TerrainApp::UpdateMainPassCB(const GameTimer& gt)
{
	// Matrix must load before using
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX invVeiw = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX viewproj = XMMatrixMultiply(view, proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewproj), viewproj);
	// after all calculate next step is store them
	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invVeiw));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewproj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mEyePos;
	// Rendering client size also is Constants need to be set in constants buffer for rendering using
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	// the far and near distance also is constants need to be set
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	// after all setdown, copy the data to current frame resource
	auto currentPassCB = mCurrentFrameResource->PassCB.get();
	currentPassCB->CopyData(0, mMainPassCB);
}

void TerrainApp::UpdateWaves(const GameTimer& gt)
{
	static float t_Base = 0.0f;
	if ((mTimer.TotalTime()) - t_Base >= 0.25f)
	{
		t_Base += 0.25f;
		int i = MathHelper::Rand(4, mWaves->RowCount() - 5);
		int j = MathHelper::Rand(4, mWaves->ColumnCount() - 5);
		float r = MathHelper::RandF(0.2f, 0.5f);
		mWaves->Disturb(i, j, r);
	}
	mWaves->Update(gt.DeltaTime()); // update Geo using Time.

	auto currentWavesVB = mCurrentFrameResource->WavesVB.get();
	for (int i = 0; i < mWaves->vertexCount(); ++i)
	{
		Vertex v;
		v.Pos = mWaves->Position(i);
		v.Color = XMFLOAT4(Colors::Chocolate);
		currentWavesVB->CopyData(i, v);
	}

	mWavesRitems->Geo->VertexBufferGPU = currentWavesVB->Resource();
}

float TerrainApp::GetHillsHeight(float x, float z)const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 TerrainApp::GetHillNormal(float x, float z)const
{
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

//---------------------------------------------Loop Function----------------------------------------------------------//

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
	UpdateWaves(gt);
}

void TerrainApp::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrentFrameResource->CmdListAlloc;
	ThrowIfFailed(cmdListAlloc->Reset());
	mCommandList->Reset(cmdListAlloc.Get(), mSceneManager->UsePSO("Default"));
	//Reset viewport
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// indicate state transition for resource usage let cpu and gpu know 
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear Render buffer and depth buffer
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// Bind per-pass constant buffer
	auto passCB = mCurrentFrameResource->PassCB->Resource();
	mCommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());


	// Now we can Draw our current pass all object
	DrawRenderItems(mCommandList.Get(), RenderLayer::Opaque);

	// after all command set
	// indicate resource state transition for resource usage let cpu and gpu both know that
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	// after done before excute need close.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdlist[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdlist), cmdlist);

	// after done excute 
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
	//add fence
	mCurrentFrameResource->Fence = ++mCurrentFence;
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void TerrainApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, RenderLayer renderlayer)
{
	UINT objCBSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = mCurrentFrameResource->ObjectCB->Resource();

	// cycle each item
	for (size_t i = 0; i < mSceneManager->RenderLayerItem(renderlayer).size(); ++i)
	{
		auto ri = mSceneManager->RenderLayerItem(renderlayer)[i];

		// set Vertex buffer
		cmdList->IASetVertexBuffers(0, 1, &(ri->Geo->VertexBufferView()));
		// set index buffer
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		// Set draw topology
		cmdList->IASetPrimitiveTopology(ri->Topology);

		// Offset object Constant GPU adress base on object index
		D3D12_GPU_VIRTUAL_ADDRESS objectCBAderss = objectCB->GetGPUVirtualAddress();

		objectCBAderss += (ri->ObjIndex) * objCBSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objectCBAderss);

		// Draw instance
		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}

}