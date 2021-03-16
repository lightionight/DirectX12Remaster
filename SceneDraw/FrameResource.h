#ifndef FRAMERESOURCE_H
#define FRAMERESOURCE_H


#include <d3dUtil.h>
#include <MathHelper.h>
#include <UploadBuffer.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct ObjectConstants
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
};

struct PassConstants
{
	XMFLOAT4X4 View = MathHelper::Identity4x4();
	XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };

	float CbPerObjectPad1 = 0.0f;
	XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct FrameResource
{
public:
	ComPtr<ID3D12CommandAllocator> CmdListAlloc;
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	
// Struct Constructor not set this value, so default is 0, using for condition Current Work is done or not;
	UINT64 Fence = 0;
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

};





#endif // !FRAMERESOURCE_H
