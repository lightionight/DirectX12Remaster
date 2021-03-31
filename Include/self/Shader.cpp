#include "Shader.h"

// Shader_MODEL_5.0
Shader::Shader(const std::string& name, const std::wstring& vspath, const std::wstring& psPath, const std::wstring& gsPath)
{
	mName = name;
	mVS = d3dUtil::CompileShader(vspath, nullptr, "vert", "vs_5_0");
	mPS = d3dUtil::CompileShader(psPath, nullptr, "frag", "ps_5_0");
	if (!gsPath.empty())
	{
		mGS = d3dUtil::CompileShader(gsPath, nullptr, "geom", "gs_5_0");
	}
}
Shader::~Shader()
{
	//mVS->Release();
	//mPS->Release();
	//mGS->Release();
}
D3D12_SHADER_BYTECODE Shader::VS()
{
	D3D12_SHADER_BYTECODE vs = { reinterpret_cast<BYTE*>(mVS->GetBufferPointer()), mVS->GetBufferSize() };
	return vs;
}
D3D12_SHADER_BYTECODE Shader::PS()
{
	D3D12_SHADER_BYTECODE ps = { reinterpret_cast<BYTE*>(mPS->GetBufferPointer()), mPS->GetBufferSize() };
	return ps;
}
D3D12_SHADER_BYTECODE Shader::GS()
{
	D3D12_SHADER_BYTECODE gs = { reinterpret_cast<BYTE*>(mGS->GetBufferPointer()), mGS->GetBufferSize() };
	return gs;
}