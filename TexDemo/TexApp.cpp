#include "TexApp.h"

const int gNumFrameResources = 3;

TexApp::TexApp() : DXApp()
{
	mSceneManager = std::make_unique<SceneManager>();
}

TexApp::~TexApp()
{
	DXApp::~DXApp();
}

void TexApp::Initialize()
{
	DXApp::Initialize();

	mDirectX->InitCommand();

	LoadTex();
	BuildShaderAndInputLayout();
	BuildGeo();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources(); // --- NEED BUILD MESH AND MATERIAL FIRST!!!!
	BuildPSO();   //----------NEED INPUTLAYOUT ROOTSIGNATURE AND BUILD SHADER FIRST!!!!

	mDirectX->CommandExcute();
}

void TexApp::LoadTex()
{
	mSceneManager->LoadTex("Brick", L"bricks.dds", &mDirectX->Device, &mDirectX->CommandList);
	mSceneManager->LoadTex("Brick2", L"bricks2.dds", &mDirectX->Device, &mDirectX->CommandList);
}

void TexApp::BuildShaderAndInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	for (auto e : inputLayout)
	{
		mInputLayout.push_back(e);
	}

	mSceneManager->AddShader("Default", L"../Shaders/Default.hlsl", L"../Shaders/Default.hlsl");
	mSceneManager->UseShader("Default");
}

void TexApp::BuildGeo()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(5.0f, 5.0f, 5.0f, 4);
	
	SubmeshGeometry boxSubmesh;

	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.BaseVertexLocation = 0;
	boxSubmesh.StartIndexLocation = 0;

	// Transform mesh to vertex Data Structure.
	std::vector<Vertex> vertices(box.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices = box.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mSceneManager->AddGeo(mDirectX->Device, mDirectX->CommandList, "Box", vbByteSize, ibByteSize, vertices, indices);

}

void TexApp::BuildMaterials()
{
	mSceneManager->AddMat("Brick", 0, 0, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);

}

void TexApp::BuildPSO()
{
	mSceneManager->AddPso("Default", mDirectX->Device, mDxBind->RootSignnature,
		mInputLayout, "Default", mDXDesc->BackBufferFormat, mDXDesc->DepthStencilFormat,
		m4xMsaaState, m4xMsaaQuality);
}

void TexApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResource; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(mDirectX->Device.Get(),
			1, (UINT)mSceneManager->AllSceneItem()->size(), (UINT)mSceneManager->GetAllMats()->size()));
	}
}

void TexApp::BuildRenderItems()
{
	auto boxRitem = std::make_unique<RenderItem>();

	boxRitem->ObjIndex = 0;
	boxRitem->Geo = mSceneManager->GetGeoPointer("Box");
	boxRitem->Mat = mSceneManager->GetMatPointer("Brick");
	boxRitem->Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	mSceneManager->AddToSceneitems(boxRitem);

	for (auto& e : *(mSceneManager->AllSceneItem()))
	{
		mSceneManager->AddToRenderLayer(RenderLayer::Opaque, e.get());
	}
}

void TexApp::OnResize()
{
	DXApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, mWinDesc->AspectRadio(), 0.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void TexApp::Update(const GameTimer& gt)
{
	OnKeyBoradInput(gt);
	UpdateCamera(gt);

	mCurrentFrameResourceIndex = (mCurrentFrameResourceIndex + 1) % gNumFrameResources;
	mCurrentFrameResource = mFrameResources[mCurrentFrameResourceIndex].get();

	if (mCurrentFrameResource->Fence != 0 && mDirectX->Fence->GetCompletedValue() < mCurrentFrameResource->Fence)
	{
		HANDLE waitHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mDirectX->Fence->SetEventOnCompletion(mCurrentFrameResource->Fence, waitHandle));
		WaitForSingleObject(waitHandle, INFINITE);
		CloseHandle(waitHandle);

	}

	AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCBs(gt);
}

void TexApp::AnimateMaterials(const GameTimer& gt)
{

}

void TexApp::UpdateObjectCBs(const GameTimer& gt)
{
	auto currObjectCB = mCurrentFrameResource->ObjectCB.get();

	for (auto& e : *mSceneManager->AllSceneItem())
	{
		if (e->NumFrameDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

			currObjectCB->CopyData(e->ObjIndex, objConstants);

			e->NumFrameDirty--;
		}
	}
}

void TexApp::UpdateMaterialCBs(const GameTimer& gt)
{
	auto currMaterialCB = mCurrentFrameResource->MaterialCB.get();

	for (auto& e : *mSceneManager->GetAllMats())
	{
		Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);
			
			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));
			
			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			mat->NumFramesDirty--;

		}
	}
}

void TexApp::UpdateMainPassCBs(const GameTimer& gt)
{
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewproj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(&proj), proj);
	XMMATRIX inViewProj = XMMatrixInverse(&XMMatrixDeterminant(&viewproj), proj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));

	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewproj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(inViewProj));

	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = mWinDesc->RendertargetSize();
	mMainPassCB.InvRenderTargetSize = mWinDesc->InvRenderTargetSize();

	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto currentPassCB = mCurrentFrameResource->PassCB.get();

	currentPassCB->CopyData(0, mMainPassCB);

}