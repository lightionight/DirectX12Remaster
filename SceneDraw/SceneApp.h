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
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*> rItems);

private:
	std::vector<std::unique_ptr<FrameResource>>


};






#endif // !SCENE_H
