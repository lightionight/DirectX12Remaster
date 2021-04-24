/////////////////////////////////////
//   This Demo simply show How to using Direct2D to draw some shape for windows Rect to Show , not have input handle deal with.
////////////////////////////////////

#include <self\Win32Context.h>
#include <self\Dx2dData.h>
#include <cstdlib>

#include <iostream>  // for output Debug infromation

LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return WinDesc::GetStaticWinDesc()->MsgProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
	RECT r;
	GetWindowRect(GetConsoleWindow(), &r);
	MoveWindow(GetConsoleWindow(), r.left, r.top, 800, 600, TRUE);
	std::cout << "Now start Program." << std::endl;

	WinDesc winDesc;
	WindowData winData;
	D2dData d2dData;
	

	winDesc.Initialize(L"Hello Direct2D", L"First Taste For Direct2D", 1280, 720, GlobalProc); // Need Add MESSAGE HANDLE FUNCTION
	winData.Initialize(&winDesc);

	d2dData = D2dData(winData.Hwnd);

	// AddColor
	d2dData.AddColor("Pink", 0.8f, 0.2f, 0.3f, 1.0f);
	d2dData.AddColor("Green", 0.2f, 0.3f, 0.77f, 1.0f);

	d2dData.PerpareDraw("Pink");
	float left = (float)(rand() % 500);
	float right = (float)(rand() % 500);
	float top = (float)(rand() % 300);
	float bottom = (float)(rand() % 500);
	d2dData.DrawRect(left, right, top, bottom);
	d2dData.AfterDraw();
	d2dData.Show();
	
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != WM_QUIT)
	{
		
		std::system("pause");
		std::cout << "Now is Countine Draw Rect as you see." << std::endl;
	}

	return 0;
}
