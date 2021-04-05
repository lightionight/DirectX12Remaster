///////////////////////////////////////////////////////////////////////////////////
////
////            Win32 Api Dependence  Initialize Context
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <iostream>

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

	void Initialize(const LPCWSTR className, const  LPCWSTR windowName, int width, int height, WNDPROC wndproc)
	{
		ClassName = className;
		WindowName = windowName;
		ClientWidth = width;
		ClientHeight = height;
		WndProc = wndproc;
	}
	float AspectRadio() const { return static_cast<float> (ClientWidth) / ClientHeight; }
} WinDesc;


typedef struct WindowData
{
	HINSTANCE Hinstance;
	HWND Hwnd = nullptr;

	WindowData()
	{
		Hinstance = GetModuleHandle(NULL);
	}

	void Initialize(const WinDesc* desc)
	{
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, desc->WndProc, NULL, NULL, Hinstance, NULL, NULL, NULL, NULL, desc->ClassName, NULL };
		RegisterClassEx(&wc);

		// Adjust Windows Size;
		RECT r = { 0 , 0 , desc->ClientWidth, desc->ClientHeight };
		::AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
		int w = r.right - r.left;
		int h = r.bottom - r.top;

		Hwnd = ::CreateWindow(desc->ClassName, desc->WindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL, Hinstance, NULL);

		::ShowWindow(Hwnd, SW_SHOW);

		UpdateWindow(Hwnd);
	}
} WindowData;