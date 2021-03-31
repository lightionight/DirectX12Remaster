#include "lightApp.h"
#include <d3dcommon.h>

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
}

void LightApp::BuildShaderAndInputLayout()
{
	const D3D_SHADER_MACRO alphaTestDefine[] =
	{
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	mSceneManager->AddShader("Standard", L"Shaders\\Default.hlsl", L"Shader\\Default.hlsl");

	mInputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0 , DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD",0 ,DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

}

void LightApp::BuildGeo()
{

}
