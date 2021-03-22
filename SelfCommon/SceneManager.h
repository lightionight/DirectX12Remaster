#pragma once

#include <d3dUtil.h>
#include <MathHelper.h>
#include <UploadBuffer.h>
#include <GeometryGenerator.h>
#include <RenderItem.h>
#include <FrameResource.h>
#include <PipelineStateCreator.h>

class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Update(); // This function using for update dymatic geometry
	void AddRenderItem(RenderItem* addItem, RenderLayer renderLayer);
	void Render(ID3D12GraphicsCommandList* cmdList);

	void AddShader(
		const std::string& name, 
		const std::wstring& vsPath, 
		const std::wstring& psPath, 
		const std::wstring& gsPath = nullptr);
	Shader* UseShader(const std::string& name);

	void UsePso(const std::wstring& name);

	void AddPso(const std::wstring& name,
	    ComPtr<ID3D12Device> currentD3dDevice,
		ComPtr<ID3D12RootSignature> currentRootSignature,
		const std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
		Shader* shader,
		DXGI_FORMAT backbufferFormat,
		DXGI_FORMAT depthStencilbufferFormat,
		bool m4xMsaa,
		UINT m4xMsaaQuality);
	void AddGeo(
		ComPtr<ID3D12Device> d3ddeivce,
		ComPtr<ID3D12GraphicsCommandList> commandList,
		const std::string& meshGeometryName,
		const std::string& drawArgs,
		GeometryGenerator::MeshData* mesh,
		const UINT vbByteSize,
		const UINT ibByteSize,
		const std::vector<Vertex>& vertices,
		const std::vector<std::uint16_t>& indices);
	void RemoveGeo();
	
private:
	std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders;
	std::unique_ptr<PipelineStateCreator> mPSOs;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeos;
	std::vector<std::unique_ptr<RenderItem>> RItems;
	std::vector<RenderItem*> mRenderLayer[(UINT)RenderLayer::Count];
	RenderLayer mDefaultRenderLayter = RenderLayer::Opaque;
	
};