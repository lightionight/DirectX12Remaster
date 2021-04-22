///////////////////////////////////////////////////////////////////////////////////
////
////            Win32 Api Dependence  Initialize Context
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <iostream>

#include <DirectXMath.h>

LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct WinDesc
{
	bool Paused = false;
	bool Minimized = false;
	bool Maximized = false;
	bool Resizing = false;
	bool FullScreen = false;
	int ClientWidth = 1920;
	int ClientHeight = 1080;
	LPCWSTR ClassName = L"DirectApp";
	LPCWSTR WindowName = L"Direct Demo";
	WNDPROC WndProc = nullptr;
	static WinDesc* staticWinDesc;
public:

	WinDesc();

	void Initialize(const LPCWSTR className, const  LPCWSTR windowName, int width, int height, WNDPROC wndproc);

	static WinDesc* GetStaticWinDesc();


	LRESULT CALLBACK MsgProc(HWND Hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//Parameter
	float AspectRadio() const
	{ 
		return static_cast<float> (ClientWidth) / ClientHeight;
	}
	const DirectX::XMFLOAT2 RendertargetSize() const
	{
		return DirectX::XMFLOAT2((float)ClientWidth, (float)ClientHeight);
	}
	const DirectX::XMFLOAT2 InvRenderTargetSize() const
	{
		return DirectX::XMFLOAT2(1.0f / ClientWidth, 1.0f / ClientHeight);
	}
} WinDesc;


typedef struct WindowData
{
	HINSTANCE Hinstance;
	HWND Hwnd = nullptr;

	static WindowData* staitcWinData ;

	WindowData();
	void Initialize(const WinDesc* desc);
} WinData;