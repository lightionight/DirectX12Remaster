#include <iostream>
#include <windows.h>

/// <summary>
/// Main files to initialize windows
/// </summary>
/// <returns></returns>

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	std::cout << "hello World Debug Console. " << std::endl;
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MainProc, NULL, NULL, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"App", NULL };
	RegisterClassEx(&wc);

	RECT r = { 0, 0, 1980, 1080 };
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);
	int width = r.right - r.left;
	int height = r.bottom - r.top;
	

	HWND hwnd = CreateWindow(wc.lpszClassName, L"Hello World", WS_OVERLAPPEDWINDOW,
		100, 100, width, height, NULL, NULL, wc.hInstance, NULL);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
	}

	return 0;

	
}

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(NULL);
		return 0;
	}

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
