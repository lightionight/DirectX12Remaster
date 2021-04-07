///////////////////////////////////////////////////////////////////////////////////
////
////            Win32 Api Dependence  Initialize Context
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <iostream>

struct WinDesc
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

	void Initialize(const LPCWSTR className, const  LPCWSTR windowName, int width, int height, WNDPROC wndproc);

	//Parameter
	float AspectRadio() const
	{ 
		return static_cast<float> (ClientWidth) / ClientHeight;
	}
};


struct WindowData
{
	HINSTANCE Hinstance;
	HWND Hwnd = nullptr;

	WindowData();
	void Initialize(const WinDesc* desc);
};