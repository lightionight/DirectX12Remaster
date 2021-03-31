#pragma once

#include <dx12book\d3dUtil.h>
#include <dx12book\MathHelper.h>
#include <dx12book\UploadBuffer.h>
#include <dx12book\GeometryGenerator.h>
#include <dx12book\FrameResource.h>

#include "RenderItem.h"
#include "PipelineStateCreator.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Update(); // This function using for update dymatic geometry
	void AddRenderItem(RenderItem* addItem, RenderLayer renderLayer);
	void AddShader(
		const std::string& name, 
		const std::wstring& vsPath, 
		const std::wstring& psPath, 
		const std::wstring& gsPath = std::wstring());
	Shader* UseShader(const std::string& name);

	ID3D12PipelineState* UsePSO(const std::string& name);
	void CreatePSO(const std::string& name);

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
		const UINT vbByteSize,
		const UINT ibByteSize,
		const std::vector<Vertex>& vertices,
		const std::vector<std::uint16_t>& indices);
	void AddGeo(
		ComPtr<ID3D12Device> d3ddeivce,
		ComPtr<ID3D12GraphicsCommandList> commandList,
		const std::string& meshGeometryName,
		const std::string& drawArgs,
		const UINT vbByteSize,
		const UINT ibByteSize,
		//const std::vector<Vertex>& vertices,
		const std::vector<std::uint16_t>& indices);
	void AddToRenderLayer(RenderLayer renderlayer, RenderItem* additem);
	void AddToSceneitems(std::unique_ptr<RenderItem>* additem);
	// Render Item  using for render layer
	std::vector<RenderItem*> RenderLayerItem(RenderLayer renderlayer);
	std::vector<std::unique_ptr<RenderItem>>* AllRenderItem();
	void AddMat(std::unique_ptr<Material>* newMaterial);
public: // Parameter
	MeshGeometry* GetGeoPointer(const std::string& geoName);
	
	
private:
	std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders; // Shader Files
	std::vector<std::unique_ptr<Material>> mMats;    // Include Materials
	std::unique_ptr<PipelineStateCreator> mPSOs;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeos;
	std::vector<std::unique_ptr<RenderItem>> mSceneItems;
	std::vector<RenderItem*> mRenderLayer[(UINT)RenderLayer::Count];
	RenderLayer mDefaultRenderLayter = RenderLayer::Opaque;
	
};