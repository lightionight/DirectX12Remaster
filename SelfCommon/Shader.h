#pragma once
//-------------------
#include <d3dUtil.h>

using Microsoft::WRL::ComPtr;

class Shader
{
public:
	Shader(std::string Name, std::wstring vsPath, std::wstring psPath, std::wstring gsPath = NULL);
	~Shader();
	std::string Name()const { return mName;}
	D3D12_SHADER_BYTECODE VS();
	D3D12_SHADER_BYTECODE PS();
	D3D12_SHADER_BYTECODE GS();
private:
	std::string mName;
	ComPtr<ID3DBlob> mVS;
	ComPtr<ID3DBlob> mPS;
	ComPtr<ID3DBlob> mGS;

	
};