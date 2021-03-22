#pragma once
#include <d3dUtil.h>
#include <MathHelper.h>

using namespace DirectX;

struct VertexBase
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct VertexCommon
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
	XMFLOAT2 Texcoord;
};

class InputData
{
public:
	InputData();
	~InputData();

private:

};

InputData::InputData()
{
}

InputData::~InputData()
{
}
