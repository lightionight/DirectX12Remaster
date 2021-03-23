#ifndef RENDER_ITEM_H
#define RENDER_ITEM_H


#include <d3dApp.h>
#include <MathHelper.h>
#include <UploadBuffer.h>
#include <PipelineStateCreator.h>
// Using
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct RenderItem
{
	RenderItem() = default;
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	int NumFrameDirty = gNumFrameResources;
	UINT ObjIndex = -1;
    // Big Mesh Pointer
	MeshGeometry* Geo = nullptr;
    // Primitive topology is draw line or draw triangle
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // This need sperate out this structure.
    // Draw Instance Parameter
	UINT IndexCount         = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation  = 0;
	//Geometry Statu
	bool _isDymatic = false;
};

// NEW ADD
enum class RenderLayer : int
{
	Opaque = 0,
	Count
};

#endif // !RENDER_ITEM_H
