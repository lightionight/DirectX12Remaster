#pragma once

#include <d3dUtil.h>
#include <d3dApp.h>

class LightApp : public D3DApp
{
public:
	LightApp(HINSTANCE hInstace);
	~LightApp();
public:
	bool Initialize()override;
};