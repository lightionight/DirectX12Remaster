#ifndef SCENE_H
#define SCENE_H

#include <d3dApp.h>
#include "FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

//Help Structure;
struct RenderItem
{
	RenderItem() = default;

	// this matrix desribe item relationship with world, include offset rotate scale
	XMFLOAT4X4 World = MathHelper::Identity4x4(); 

	int NumFrameDirty = gNumFrameResources;
	UINT objCBIndex = -1;

	MeshGeometry* geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawInstance parameters
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	UINT baseVertexLocation = 0;
};






class SceneApp : public D3DApp
{
public:
	SceneApp(HINSTANCE hInstance);
	SceneApp(const SceneApp& rhs) = delete;
	SceneApp& operator=(const SceneApp& rhs) = delete;

	~SceneApp();

	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyBoardInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);

	void BuildDescriptorHeaps();
	void BuildConstantBufferViews();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*> rItems);

private:
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrentFrameResources = nullptr;
	int mCurrentFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
	
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

// This Contains lots of data using Container to included. 
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;  // batch of Geometry
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;       // batch of Shader id3dblob
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;   // batch of d3d render pipeline state ojbect pointers

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;  // vector array of inputlayout desc
// All  RenderItems
	std::vector<std::unique_ptr<RenderItem>> mAllRItems;
	std::vector<RenderItem*> mOpaqueRitems;

	PassConstants mMainPassCB;
	UINT mPassCbvOffset = 0;

// Position Variables
	bool isWireFrame = false;
	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f }; // Camera Postion
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
// Speherial
	float mRadius = 15.0f;
	float mPhi = 0.2f * XM_PI;
	float mTheta = 1.5f * XM_PI;

// Mouse Position
	POINT mLastMousePos;
};






#endif // !SCENE_H
