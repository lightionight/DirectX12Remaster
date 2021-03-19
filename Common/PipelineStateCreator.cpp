#include "PipelineStateCreator.h"

PipelineStateCreator::PipelineStateCreator(D3D12_INPUT_LAYOUT_DESC* inputLayout, 
        ID3D12Device* currentd3dDevice,
        ID3D12RootSignature* currentRootSignature,
        ID3DBlob* VS,
        ID3D10Blob* PS)
        {
            // Need an string to condition what type pipeline state for current using
            // or Just Create all pipeline state wait for using select.

            BuildRegularPipelineState();

        }
PipelineStateCreator::~PipelineStateCreator() { }

void 
PipelineStateCreator::BuildRegularPipelineState(
    ID3D12Device** currentD3dDevice,
    ID3D12RootSignature** currentRootSignature,
    D3D12_INPUT_LAYOUT_DESC** inputLayout,
    ID3DBlob** VS,
    ID3DBlob** PS
)
{
    // PipelineStateOjbect Need 
    //                   Inputlayout  
    //                   RootSignature, 
    //                   VS, PS, 
    //                   PrimivtiveTopologyType
    //                   D3DDevice 
    //                   for create it.
    D3D12_GRAPHICS_PIPELINE_STATE_DESC regularPsoDesc;
    ZeroMemory(&regularPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));


}