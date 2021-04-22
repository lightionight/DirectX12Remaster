#include <self/Win32Context.h>

//______________________WIN DESC

// Set Default is nullptr
WinDesc* WinDesc::staticWinDesc = nullptr;

WinDesc::WinDesc()
{
	if (staticWinDesc == nullptr)
		staticWinDesc = this;
}

void WinDesc::Initialize(const LPCWSTR className, const  LPCWSTR windowName, int width, int height, WNDPROC wndproc)
{
	ClassName = className;
	WindowName = windowName;
	ClientWidth = width;
	ClientHeight = height;
	WndProc = wndproc;
}
WinDesc* WinDesc::GetStaticWinDesc()
{
	return staticWinDesc;
}


LRESULT CALLBACK WinDesc::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}



//______________________WIN DATA

WindowData::WindowData()
{
	Hinstance = GetModuleHandle(NULL);
}

void WindowData::Initialize(const WinDesc* desc)
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

