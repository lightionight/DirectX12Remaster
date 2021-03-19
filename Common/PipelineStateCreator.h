/////////////////////////////////////////////
// 
//    This cLass main include how to build pipelineStateObject As easy As it can
//
/////////////////////////////////////////////



#ifndef PIPELINE_STATE_CREATOR_H
#define PIPELINE_STATE_CREATOR_H

#include <d3d12.h>
#include "d3dUtil.h"
#include 
using Microsoft::WRL::ComPtr;

class PipelineStateCreator
{
    //Constructor
    public:
    PipelineStateCreator(D3D12_INPUT_LAYOUT_DESC* inputLayout, 
        ID3D12Device* currentd3dDevice,
        ID3D12RootSignature* currentRootSignature,
        ID3DBlob* VS,
        ID3D10Blob* PS);
    ~PipelineStateCreator();
    private:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC* mCurrentPsoDesc = nullptr;
    ID3D12PipelineState* mCurrentPso = nullptr;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
    public:
    void SwiftPSO(std::string name);
    void Initialize();
    void BuildRegularPipelineState(ID3D12Device** currentD3dDevice,
        ID3D12RootSignature** currentRootSignature,
        D3D12_INPUT_LAYOUT_DESC** inputLayout,
        ID3DBlob** VS,
        ID3DBlob** PS
    );
    //paramter
    public:
    const ID3D12PipelineState* CurrentPipelineState () const {return mCurrentPso;}
    

}




#endif  // ! PIPELINE_STATE_CREATOT_H