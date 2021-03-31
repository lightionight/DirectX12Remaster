/////////////////////////////////////////////
// 
//    This cLass main include how to build pipelineStateObject As easy As it can
//
/////////////////////////////////////////////



#ifndef PIPELINE_STATE_CREATOR_H
#define PIPELINE_STATE_CREATOR_H

#include <d3d12.h>
#include <dx12book\d3dUtil.h>
#include "Shader.h"

using Microsoft::WRL::ComPtr;

class PipelineStateCreator
{
    //Constructor
public:
    PipelineStateCreator( );
    ~PipelineStateCreator( );
private:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC* mCurrentPsoDesc = nullptr;
    ID3D12PipelineState* mCurrentPso = nullptr;
    std::unordered_map<std::string, ID3D12PipelineState*> PSOLibrary;
    //ID3D12PipelineLibrary
    //ID3D12PipelineLibrary* mPSOLibrary = nullptr;
public:
    void CreatePipelineState(
             const std::wstring& name,
             ComPtr<ID3D12Device> currentd3dDevice,
             ComPtr<ID3D12RootSignature> currentRootSignature,
             const std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
             Shader* shader,
             DXGI_FORMAT backBufferFormat,
             DXGI_FORMAT depthStencilBufferFormat,
             bool m4xMsaa,
             UINT m4xMsaaQuality);
    
    void BuildRegularPipelineState(
            ComPtr<ID3D12Device> currentd3dDevice,
            ComPtr<ID3D12RootSignature> currentRootSignature,
            const std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
            Shader* shader,
            DXGI_FORMAT backBufferFormat,
            DXGI_FORMAT depthStencilBufferFormat,
            bool m4xMsaa,
            INT m4xMsaaQuality);

    ID3D12PipelineState* SwiftPSO(const std::string& name);
    void Initialize();
    //paramter
public:
    const ID3D12PipelineState* CurrentPipelineState() const { return mCurrentPso; }
    ID3D12PipelineState* DefaultPSO() { return PSOLibrary["Default"]; }

    //ComPtr<ID3D12PipelineLibrary>


};




#endif  // ! PIPELINE_STATE_CREATOT_H