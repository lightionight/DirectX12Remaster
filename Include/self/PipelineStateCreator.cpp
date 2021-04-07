#include "PipelineStateCreator.h"

PipelineStateCreator::PipelineStateCreator() {

}
PipelineStateCreator::~PipelineStateCreator() { }

void PipelineStateCreator::BuildRegularPipelineState(
    ComPtr<ID3D12Device> currentD3dDevice,
    ComPtr<ID3D12RootSignature> currentRootSignature,
    const std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
    Shader* shader,
    DXGI_FORMAT backBufferFormat,
    DXGI_FORMAT depthStencilBufferFormat,
    bool m4xMsaa,
    INT m4xMsaaQuality)
{
    // PipelineStateOjbect Need 
    //                   Inputlayout  
    //                   RootSignature, 
    //                   Shader -> Contain VS And PS ID3dblob
    //                   PrimivtiveTopologyType
    //                   D3DDevice 
    //                   for create it.
    D3D12_GRAPHICS_PIPELINE_STATE_DESC regularPsoDesc;
    ZeroMemory(&regularPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    regularPsoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
    regularPsoDesc.pRootSignature = currentRootSignature.Get();
    regularPsoDesc.VS = shader->VS();
    regularPsoDesc.PS = shader->PS();
    regularPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    regularPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    regularPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    regularPsoDesc.SampleMask = UINT_MAX;
    regularPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    regularPsoDesc.NumRenderTargets = 1;
    regularPsoDesc.RTVFormats[0] = backBufferFormat;
    regularPsoDesc.SampleDesc.Count = m4xMsaa ? 4 : 1;
    regularPsoDesc.SampleDesc.Quality = m4xMsaa ? (m4xMsaaQuality - 1) : 0;
    regularPsoDesc.DSVFormat = depthStencilBufferFormat;
    regularPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    // now Using d3ddevice to Create pipeline state object
    currentD3dDevice->CreateGraphicsPipelineState(&regularPsoDesc, IID_PPV_ARGS(&PSOLibrary["Default"]));
}

void PipelineStateCreator::CreatePipelineState(
    const std::string& name,
    ComPtr<ID3D12Device> currentd3dDevice,
    ComPtr<ID3D12RootSignature> currentRootSignature,
    const std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
    Shader* shader,
    DXGI_FORMAT backBufferFormat,
    DXGI_FORMAT depthStencilBufferFormat,
    bool m4xMsaa,
    UINT m4xMsaaQuality)
{
    if (name != "Default")
    {
        CreatePipelineState(name,
            currentd3dDevice,
            currentRootSignature,
            inputLayout,
            shader,
            backBufferFormat,
            depthStencilBufferFormat,
            m4xMsaa,
            m4xMsaaQuality);
    }
    else
    {
        BuildRegularPipelineState(currentd3dDevice,
            currentRootSignature,
            inputLayout,
            shader,
            backBufferFormat,
            depthStencilBufferFormat,
            m4xMsaa,
            m4xMsaaQuality);
    }
}

ID3D12PipelineState* PipelineStateCreator::SwiftPSO(const std::string& name)
{
    if (PSOLibrary[name])
    {
        mCurrentPso = PSOLibrary[name];
    }
    else
    {
        mCurrentPso = PSOLibrary["Default"];
        // or Set PSO to Error PSO
    }
    return mCurrentPso;
}