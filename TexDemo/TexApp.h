#pragma once

#include <self/DxApp.h>
#include <self/SceneManager.h>
#include <dx12book/FrameResource.h>

class TexApp: public DXApp
{
public:
	TexApp();
	~TexApp();
public:
public:
	virtual void Initialize()override;

private:
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;
	virtual void OnResize()override;

	virtual void OnRotate(int x, int y)override;
	virtual void OnScale(int x, int y)override;
	virtual void OnMouseMove(int x, int y)override;

	void LoadTex();
	void BuildShaderAndInputLayout();
	void BuildGeo();
	void BuildSkullGeometry();
	void BuildPSO();
	void BuildFrameResources();
	void BuildMaterials();
	void BuildRenderItems();
	void PerPassDrawItems();

	void OnKeyBoradInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void AnimateMaterials(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMaterialCBs(const GameTimer& gt);
	void UpdateMainPassCBs(const GameTimer& gt);

protected:
	std::unique_ptr<SceneManager> mSceneManager;

	//Frame Resource
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrentFrameResource = nullptr;
	int mCurrentFrameResourceIndex = 0;

	//InputLayOut
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	PassConstants mMainPassCB;

	float mTheta = 1.5f * XM_PI;
	float mPhi = 0.2f * XM_PI;
	float mRadius = 15.0f;

	POINT mMousePos;
};

