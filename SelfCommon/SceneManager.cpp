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

void SceneManager::Render(ID3D12GraphicsCommandList)
{

}

void SceneManager::AddGeo(
	ComPtr<ID3D12Device> d3ddeivce,
	ComPtr<ID3D12GraphicsCommandList> commandList,
	const std::string& meshGeometryName,
	const std::string& drawArgs,
	GeometryGenerator::MeshData* mesh,
	const UINT vbByteSize,
	const UINT ibByteSize,
	const std::vector<Vertex>& vertices,
	const std::vector<std::uint16_t>& indices)
{
	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = meshGeometryName;
	D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU);
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
	D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU);
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
	//Copy CPU-> GPU
	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(
		d3ddeivce.Get(),
		commandList.Get(),
		vertices.data(),
		vbByteSize,
		geo->VertexBufferUploader);
	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(
		d3ddeivce.Get(),
		commandList.Get(),
		indices.data(),
		ibByteSize,
		geo->IndexBufferUploader);
	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.BaseVertexLocation = 0;
	submesh.StartIndexLocation = 0;
	geo->DrawArgs[drawArgs] = submesh;
	mGeos[meshGeometryName] = std::move(geo);
}