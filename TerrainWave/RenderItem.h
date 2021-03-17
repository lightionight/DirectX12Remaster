#pragma once

#include <d3dApp.h>
#include <MathHelper.h>
#include <UploadBuffer.h>

const int gNumFrameResources = 3;

// Using
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct RenderItem
{
	RenderItem() = default;
// WorldMatrix
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	int NumFrameDirty = gNumFrameResources;
	UINT ObjIndex = -1;
// Big Mesh Pointer
	MeshGeometry* Geo = nullptr;
// Primitive topology is draw line or draw triangle
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

// Draw Instance Parameter
	UINT IndexCount         = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation  = 0;
};

// NEW ADD
enum class RenderLayer : int
{
	Opaque = 0,
	Count
};

