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

	void AddPso(const std::string& name,
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
		const std::vector<std::uint32_t>& indices);
	void AddGeo(
		ComPtr<ID3D12Device> d3ddeivce,
		ComPtr<ID3D12GraphicsCommandList> commandList,
		const std::string& meshGeometryName,
		const std::string& drawArgs,
		const UINT vbByteSize,
		const UINT ibByteSize,
		//const std::vector<Vertex>& vertices,
		const std::vector<std::uint16_t>& indices);

	// this function is using add big mesh data but not add draw mesh
	template <typename T>
	void AddGeo(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandlist,
		const std::string& meshName, const UINT vbByteSize, const UINT ibByteSzie,
		const std::vector<Vertex>& vertices, const std::vector<T>& indices);

	void AddDraw(const std::string& GeometryName, const std::string& DrawName, const SubmeshGeometry& drawMesh);

	void AddToRenderLayer(RenderLayer renderlayer, RenderItem* additem);
	void AddToSceneitems(std::unique_ptr<RenderItem>* additem);
	// Render Item  using for render layer
	std::vector<RenderItem*> RenderLayerItem(RenderLayer renderlayer);
	std::vector<std::unique_ptr<RenderItem>>* AllRenderItem();
	void AddMat(const std::string& name, const int cbIndex, const int diffHeapIndex, const XMFLOAT4& diffuse, const XMFLOAT3& fresnel, const float roughness);
	
public: // Parameter
	MeshGeometry* GetGeoPointer(const std::string& geoName);
	Material* GetMatPointer(const std::string& matName);
	std::unordered_map<std::string, std::unique_ptr<Material>>* GetAllMats();
	size_t MaterialCount();

	
	
private:
	std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders; // Shader Files
	std::vector<std::unique_ptr<Material>> mAllMats;    // Include Materials
	std::unique_ptr<PipelineStateCreator> mPSOs;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeos;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMats;

	std::vector<Material*> mAllMaterial;
	std::vector<std::unique_ptr<RenderItem>> mSceneItems;
	std::vector<RenderItem*> mRenderLayer[(UINT)RenderLayer::Count];
	RenderLayer mDefaultRenderLayter = RenderLayer::Opaque;
	
};