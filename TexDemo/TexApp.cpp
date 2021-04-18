#include "TexApp.h"

const int gNumFrameResource = 3;

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
	BuildFrameResources();
	BuildPSO();

	mDirectX->CommandExcute();
}

void TexApp::LoadTex()
{
	auto woodCrateTex = std::make_unique<Texture>();
	woodCrateTex->Name = "WoodTexture";
}
