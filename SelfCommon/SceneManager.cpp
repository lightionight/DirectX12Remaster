#include <SceneManager.h>

SceneManager::SceneManager()
{
	//mPSOs = std::make_unique<PipelineStateCreator>();
}
SceneManager::~SceneManager() { }

void SceneManager::Update()
{
	for (int i = 0; i < mRenderLayer[(UINT)mDefaultRenderLayter].size(); ++i)
	{
		auto rItem = mRenderLayer[(UINT)mDefaultRenderLayter][i];
		if (rItem->_isDymatic)
		{
			
		}
	}
}

// name = Shader Name, VsPath = vertex shader path, psPath = pixel Shader path
void SceneManager::AddShader(const std::string& name, const std::wstring& vsPath, const std::wstring& psPath, const std::wstring& gsPath = nullptr)
{
	if (gsPath.empty())
	{
		mShaders[name] = std::make_unique<Shader>(name, vsPath, psPath);
	}
	mShaders[name] = std::make_unique<Shader>(name, vsPath, psPath);
}

void SceneManager::UsePso(const std::wstring& Name)
{
	mPSOs->SwiftPSO(Name);
}
void SceneManager::AddPso(
	const std::wstring& name,
	ComPtr<ID3D12Device> currentD3dDevice,
	ComPtr<ID3D12RootSignature> currentRootSignature,
	const std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
	Shader* shader,
	DXGI_FORMAT backbufferFormat,
	DXGI_FORMAT depthStencilbufferFormat,
	bool m4xMsaa,
	UINT m4xMsaaQuality)
{
	mPSOs->CreatePipelineState(name, currentD3dDevice,
		currentRootSignature, 
		inputLayout, 
		shader, 
		backbufferFormat, 
		depthStencilbufferFormat,
		m4xMsaa, 
		m4xMsaaQuality);
}

Shader* SceneManager::UseShader(const std::string& name)
{
	return mShaders[name].get();
}