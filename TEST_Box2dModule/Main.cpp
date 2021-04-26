
#include <windowsx.h>
#include <Windows.h>
#include <iostream>
#include <self\Win32Context.h>

// Using For Box2d Module Test.
#include <self\Box2dEngine.h>

LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return WinDesc::GetStaticWinDesc()->MsgProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
	RECT r;
	GetWindowRect(GetConsoleWindow(), &r);
	MoveWindow(GetConsoleWindow(), r.left, r.top, 800, 600, TRUE);

	WinDesc winDesc;
	WindowData winData;

	winDesc.Initialize(L"Hello Direct2D", L"First Taste For Direct2D", 1280, 720, GlobalProc); // Need Add MESSAGE HANDLE FUNCTION
	winData.Initialize(&winDesc);

	Box2dEngine box2dEngine(-10.0f);

	box2dEngine.Initialize();
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != WM_QUIT)
	{
		
		std::system("pause");
		std::cout << "Now is Direct2d is ready for you draw object" << std::endl;
	}

	return 0;
}