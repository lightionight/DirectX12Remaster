#include "lightApp.h"
#include <d3dcommon.h>
#include <iostream>   // for std::cout
#include <fstream>  // for std::ifsteam

const int gNumFrameResources = 3;

LightApp::LightApp() : DXApp()
{
	mSceneManager = std::make_unique<SceneManager>();
}

LightApp::~LightApp() 
{
	DXApp::~DXApp();
}

void LightApp::Initialize()
{
	DXApp::Initialize();

	BuildShaderAndInputLayout();
	BuildGeo();
	BuildSkullGeometry();
	BuildPSO();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
}

void LightApp::BuildShaderAndInputLayout()
{
	const D3D_SHADER_MACRO alphaTestDefine[] =
	{
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	mSceneManager->AddShader("Default", L"Shaders\\Default.hlsl", L"Shaders\\Default.hlsl");

	mInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD",0 ,DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

}

void LightApp::BuildGeo()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + sphere.Indices32.size();

	SubmeshGeometry boxSubmesh;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();

	SubmeshGeometry gridSubmesh;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.IndexCount = grid.Indices32.size();

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
	sphereSubmesh.IndexCount = sphere.Indices32.size();

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.IndexCount = cylinder.Indices32.size();

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
		vertices[k].Normal = box.Vertices[i].Normal;
	}
	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
	}
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
	}
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbBytesize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	mSceneManager->AddGeo(mDirectX->Device, mDirectX->CommandList, "Scene", vbBytesize, ibByteSize, vertices, indices);
	mSceneManager->AddDraw("Scene", "box", boxSubmesh);
	mSceneManager->AddDraw("Scene", "grid", gridSubmesh);
	mSceneManager->AddDraw("Scene", "sphere", sphereSubmesh);
	mSceneManager->AddDraw("Scene", "cylinder", cylinderSubmesh);
}

void LightApp::BuildSkullGeometry()
{
	std::ifstream fin("Models/skull.txt");

	if (!fin)
	{
		std::cout << "Skull Files not find" << std::endl;
		return;
	}

	UINT vCount = 0; // vertex Count
	UINT tCount = 0; // triangle Count
	std::string ignore;

	fin >> ignore >> vCount;
	fin >> ignore >> tCount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex> vertices(vCount);
	for (UINT i = 0; i < vCount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	std::vector<std::uint32_t> indices((3 * tCount));
	for (UINT i = 0; i < tCount; ++i)
	{
		fin >> indices[(i * 3) + 0] >> indices[(i * 3) + 1] >> indices[(i * 3) + 2];
	}

	fin.close();

	// Count Size 
	const UINT vbByteSize = sizeof(Vertex) * (UINT)vertices.size();
	const UINT ibByteSize = sizeof(std::int32_t) * (UINT)indices.size();

	mSceneManager->AddGeo(mDirectX->Device,mDirectX->CommandList,
		"SkullGeo", "Skull", vbByteSize, ibByteSize, vertices, indices);
}

void LightApp::BuildPSO()
{
	mSceneManager->AddPso("Default", mDirectX->Device, mDxBind->RootSignnature, mInputLayout,
		mSceneManager->UseShader("Default"), mDXDesc->BackBufferFormat, mDXDesc->DepthStencilFormat, m4xMsaaState, m4xMsaaQuality);
}

void LightApp::BuildMaterials()
{
	mSceneManager->AddMat("bricks0", 0, 0, XMFLOAT4(Colors::ForestGreen), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.1f);
	mSceneManager->AddMat("Stone0", 1, 1, XMFLOAT4(Colors::LightGray), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f);
	mSceneManager->AddMat("tiles0", 2, 2, XMFLOAT4(Colors::LightBlue), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.2f);
	mSceneManager->AddMat("SkullMat", 3, 3, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f);
}

void LightApp::BuildRenderItems()
{
	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	boxRitem->ObjIndex = 0;
	boxRitem->Mat = mSceneManager->GetMatPointer("Stone0");
	boxRitem->Geo = mSceneManager->GetGeoPointer("Scene");
	boxRitem->Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mSceneManager->AddToSceneitems(boxRitem);

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	gridRitem->ObjIndex = 1;
	gridRitem->Mat = mSceneManager->GetMatPointer("tiles0");
	gridRitem->Geo = mSceneManager->GetGeoPointer("Scene");
	gridRitem->Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	mSceneManager->AddToSceneitems(gridRitem);

	auto skullRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&skullRitem->World, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	skullRitem->TexTransform = MathHelper::Identity4x4();
	skullRitem->ObjIndex = 2;
	skullRitem->Geo = mSceneManager->GetGeoPointer("SkullGeo");
	skullRitem->Mat = mSceneManager->GetMatPointer("SkullMat");
	skullRitem->Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skullRitem->IndexCount = skullRitem->Geo->DrawArgs["Skull"].IndexCount;
	skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["Skull"].BaseVertexLocation;
	skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["Skull"].StartIndexLocation;
	mSceneManager->AddToSceneitems(skullRitem);

	XMMATRIX brickTexTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	UINT objCBindex = 3;
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCyWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCyWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, leftCyWorld);
		XMStoreFloat4x4(&leftCylRitem->TexTransform, brickTexTransform);
		leftCylRitem->ObjIndex = objCBindex++;
		leftCylRitem->Mat = mSceneManager->GetMatPointer("bricks0");
		leftCylRitem->Geo = mSceneManager->GetGeoPointer("Scene");
		leftCylRitem->Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, rightCyWorld);
		rightCylRitem->TexTransform = MathHelper::Identity4x4();
		rightCylRitem->ObjIndex = objCBindex++;
		rightCylRitem->Mat = mSceneManager->GetMatPointer("bricks0");
		rightCylRitem->Geo = mSceneManager->GetGeoPointer("Scene");
		rightCylRitem->Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
		
		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->TexTransform = MathHelper::Identity4x4();
		leftSphereRitem->ObjIndex = objCBindex++;
		leftSphereRitem->Mat = mSceneManager->GetMatPointer("Stone0");
		leftSphereRitem->Geo = mSceneManager->GetGeoPointer("Scene");
		leftSphereRitem->Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;


		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->TexTransform = MathHelper::Identity4x4();
		rightSphereRitem->ObjIndex = objCBindex++;
		rightSphereRitem->Mat = mSceneManager->GetMatPointer("Stone0");
		rightSphereRitem->Geo = mSceneManager->GetGeoPointer("Scene");
		rightSphereRitem->Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		mSceneManager->AddToSceneitems(leftCylRitem);
		mSceneManager->AddToSceneitems(rightCylRitem);
		mSceneManager->AddToSceneitems(leftSphereRitem);
		mSceneManager->AddToSceneitems(rightSphereRitem);
	}

	for (auto& e : *mSceneManager->AllRenderItem())
	{
		mSceneManager->AddToRenderLayer(RenderLayer::Opaque, e.get());
	}
}

void LightApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(mDirectX->Device.Get(),
			1, (UINT)mSceneManager->AllRenderItem()->size(), (UINT)mSceneManager->MaterialCount()));
	}
}

void LightApp::PerPassDrawItems()
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = mCurrentFrameResource->ObjectCB->Resource();
	auto matCB = mCurrentFrameResource->MaterialCB->Resource();

	for (size_t i = 0; i < mSceneManager->RenderLayerItem(RenderLayer::Opaque).size(); ++i)
	{
		auto ri = mSceneManager->RenderLayerItem(RenderLayer::Opaque)[i];

		mDirectX->CommandList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		mDirectX->CommandList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		mDirectX->CommandList->IASetPrimitiveTopology(ri->Topology);

		D3D12_GPU_VIRTUAL_ADDRESS objCbAddress = objectCB->GetGPUVirtualAddress() + ri->ObjIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + (UINT)ri->Mat->MatCBIndex * matCBByteSize;

		// For Per Object Draw add root signature constant buffer [1] [2]
		mDirectX->CommandList->SetGraphicsRootConstantBufferView(0, objCbAddress);
		mDirectX->CommandList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		mDirectX->CommandList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void LightApp::Draw(const GameTimer& gt)
{
	mDirectX->PrepareRender(mSceneManager->UsePSO("Default"), mCurrentFrameResource->CmdListAlloc);

	// Set rootSignature and create 3 constant buffer
	mDirectX->CommandList->SetGraphicsRootSignature(mDxBind->RootSignnature.Get());
	// Add root signature constant buffer[3]
	mDirectX->CommandList->SetGraphicsRootConstantBufferView(2, mCurrentFrameResource->PassCB->Resource()->GetGPUVirtualAddress());
	
	PerPassDrawItems();

	mDirectX->AfterRender(mCurrentFrameResource);
}

void LightApp::Update(const GameTimer& gt)
{
	OnKeyBoradInput(gt);
	UpdateCamera(gt);
	mCurrentFrameResourceIndex = (mCurrentFrameResourceIndex + 1) % gNumFrameResources;
	mCurrentFrameResource = mFrameResources[mCurrentFrameResourceIndex].get();

	if (mCurrentFrameResource->Fence != 0 && mDirectX->Fence->GetCompletedValue() < mCurrentFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mDirectX->Fence->SetEventOnCompletion(mCurrentFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCBs(gt);
}

void LightApp::OnKeyBoradInput(const GameTimer& gt)
{

}

void LightApp::UpdateCamera(const GameTimer& gt)
{
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.y = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.z = mRadius * cosf(mPhi);

	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void LightApp::AnimateMaterials(const GameTimer& gt)
{

}

void LightApp::UpdateObjectCBs(const GameTimer& gt)
{
	auto currentObjectCB = mCurrentFrameResource->ObjectCB.get();
	for (auto& e : *mSceneManager->AllRenderItem())
	{
		XMMATRIX world = XMLoadFloat4x4(&e->World);
		XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

		ObjectConstants objCB;
		XMStoreFloat4x4(&objCB.World, XMMatrixTranspose(world));
		XMStoreFloat4x4(&objCB.TexTransform, XMMatrixTranspose(texTransform));

		currentObjectCB->CopyData(e->ObjIndex, objCB);

		e->NumFrameDirty--;
	}
}

void LightApp::UpdateMaterialCBs(const GameTimer& gt)
{
	auto currentMaterialCB = mCurrentFrameResource->MaterialCB.get();

	for (auto& mats : *mSceneManager->GetAllMats())
	{
		Material* e = mats.second.get();
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&e->MatTransform);
			MaterialConstants matCB;
			matCB.DiffuseAlbedo = e->DiffuseAlbedo;
			matCB.FresnelR0 = e->FresnelR0;
			matCB.Roughness = e->Roughness;
			XMStoreFloat4x4(&matCB.MatTransform, XMMatrixTranspose(matTransform));
			currentMaterialCB->CopyData(e->MatCBIndex, matCB);

			e->NumFramesDirty--;
		}
	}
}

void LightApp::UpdateMainPassCBs(const GameTimer& gt)
{
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
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

	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mWinDesc->ClientWidth, (float)mWinDesc->ClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mWinDesc->ClientWidth, 1.0f / mWinDesc->ClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.25f, 0.25f, 0.25f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };

	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.3f, 0.3f, 0.3f };

	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto CurrentCB = mCurrentFrameResource->PassCB.get();
	CurrentCB->CopyData(0, mMainPassCB);
}

void LightApp::OnResize()
{
	DXApp::OnResize();
	// Update Project Matrix
	XMMATRIX Proj = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, mWinDesc->AspectRadio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, Proj);

}

void LightApp::OnRotate(int x, int y)
{
	float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mMousePos.x));
	float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mMousePos.y));
	
	mTheta +=  dx;
	mPhi += dy;

	mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);

}

void LightApp::OnScale(int x, int y)
{
	float dx = 0.05f * static_cast<float>(x - mMousePos.x);
	float dy = 0.05f * static_cast<float>(y - mMousePos.y);

	mRadius += dx - dy;

	mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
}

void LightApp::OnMouseMove(int x, int y)
{
	mMousePos.x = x;
	mMousePos.y = y;
}


