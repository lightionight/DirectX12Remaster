#pragma once
//-------------------
#include <dx12book\d3dUtil.h>

using Microsoft::WRL::ComPtr;

class Shader
{
public:
	Shader(const std::string& Name, const std::wstring& vsPath, const std::wstring& psPath, const std::wstring& gsPath = std::wstring());
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