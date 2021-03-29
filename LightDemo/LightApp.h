#pragma once


#include <d3dApp.h>
#include "Data.h"

class LightApp : public D3DApp
{
public:
	LightApp(HINSTANCE hInstace);
	~LightApp();
public:
	bool Initialize()override;
protected:
	std::unique_ptr<WindowData> mWindow;
	std::unique_ptr<DirectData> mDirectX;
	WindowStatu mWinStatu;

	GameTimer mTimer;

};