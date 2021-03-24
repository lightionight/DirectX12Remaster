#include <iostream>
#include <windows.h>

/// <summary>
/// Main files to initialize windows
/// </summary>
/// <returns></returns>

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = MainProc;
	wc.lpszClassName = L"App";
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadIcon(0, IDC_ARROW);
	wc.cbWndExtra = 0;
	wc.cbClsExtra = 0;

	RegisterClass(&wc);

	RECT r = { 0, 0, 1980, 1080 };
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
	int width = r.right - r.left;
	int height = r.bottom - r.top;

	HWND hwnd = CreateWindow(L"App", L"Hello World", WS_OVERLAPPEDWINDOW,
		CW_DEFAULT, CW_DEFAULT, width, height, NULL, NULL, wc.hInstance, NULL);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg = { 0 };
	while ((int)msg.wParam != WM_DESTROY)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;

	
}

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
