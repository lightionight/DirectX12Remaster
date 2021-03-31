#ifndef LIGHT_APP_H
#define LIGHT_APP_H

#include <self\DxApp.h>
#include <self\SceneManager.h>
#include <dx12book\FrameResource.h>

class LightApp : public DXApp
{
public:
	LightApp();
	~LightApp();
public:
	virtual void Initialize()override;
	virtual void Update(GameTimer& gt)override;
	virtual void Draw(GameTimer& gt)override;
public:
	void BuildShaderAndInputLayout();
	void BuildGeo();

protected:
	std::unique_ptr<SceneManager> mSceneManager;
	
	//Frame Resource
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrentFrameResource = nullptr;
	int mCurrentFrameResourceIndex = 0;

	//InputLayOut
	std::vector<D3D12_INPUT_LAYOUT_DESC> mInputLayout;

	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = 0.2f * XM_PI;
	float mRadius = 15.0f;

	POINT mMousePos;
};









#endif // !LIGHT_APP_H
