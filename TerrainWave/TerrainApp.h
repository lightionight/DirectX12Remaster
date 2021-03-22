#ifndef TERRAIN_APP_H
#define TERRAIN_APP_H

const int gNumFrameResources = 3;

#include <d3dApp.h>
#include <MathHelper.h>
#include <UploadBuffer.h>
#include <GeometryGenerator.h>
#include "Wave.h"
#include "RenderItem.h"
#include "FrameResource.h"
#include <SceneManager.h>
#include <Shader.h>

// Using
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class TerrainApp : public D3DApp
{
public:
// Constructor Function
	TerrainApp(HINSTANCE hInstance);
	TerrainApp(const TerrainApp& rhs) = delete;
	TerrainApp& operator=(const TerrainApp& rhs) = delete;
	~TerrainApp();
// Initialize Function
	virtual bool Initialize()override;

private:
// Ihert Function
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override; // MainLoop Function
	virtual void Draw(const GameTimer& gt)override;   // MainLoop Function
	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;
// Custom Fucntion basely Need loop per Frame
	void OnkeyBoardInput(const GameTimer& gt);
	void UpdateCameraPosition(const GameTimer& gt);
	void UpdateObjectsCB(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);
	void UpdateWaves(const GameTimer& gt);
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& rItems);
// Builds Function Runing once
	void BuildRootSignature();
	void BuildShaderAndInputLayout();
	void BuildDescriptorHeap();
	void BuildLandGeometry();
	void BuildWaterGeometry();
	void BuildFrameResource();
	void BuildPSOs();
	void BuildRenderItems();
// base on  x z get y
	float GetHillsHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;

private:
	std::unique_ptr<SceneManager> mSceneManager;
	FrameResource* mCurrentFrameResource;
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	int mCurrentFrameResourceIndex = 0;
	UINT mCbvSrvDescriptorSize = 0;
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	//std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometry;
	//std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	//std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
// This very Interesting
	RenderItem* mWavesRitems = nullptr;
	std::vector<std::unique_ptr<RenderItem>> mAllRItems;
	//std::vector<RenderItem*> mRItemLayer[(int)RenderLayer::Count];
    std::unique_ptr<Wave> mWaves;
	PassConstants mMainPassCB;
	bool _IsWireFrame = false;
// Contain  three matrix
	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();
// Self Position Calculate need Parameter
	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV2 - 0.1f;
	float mRadius = 50.0f;
// Why need sun 
	float mSunTheta = 1.25f * XM_PI;
	float mSunPhi = XM_PIDIV4;
	POINT mMousePos;  // not init
};





#endif // !TERRAIN_APP_H
