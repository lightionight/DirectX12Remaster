/////////////////////////////////////
//   This Demo simply show How to using Direct2D to draw some shape for windows Rect to Show , not have input handle deal with.
////////////////////////////////////

#include <self\Win32Context.h>
#include <self\Dx2dData.h>
#include <cstdlib>

//#include <self/Box2dSceneManager.h>
//#include <self/Box2dEngine.h>

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

#ifdef _TEST_DRAW_
	
#endif // _TEST_DRAW_
	float bottom = 0.0f;
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != WM_QUIT)
	{
		
		d2dData.PerpareDraw(D2D1::ColorF::Red);
		float left = 10.0f;
		float right = 300.0f;
		float top =  50.0f;
		bottom = bottom + 10.0f;
		D2D_POINT_2F first = { 640.0f, 360.0f };
		D2D_POINT_2F last = { 1200.0f, 360.0f };

		D2D_POINT_2F first2 = { 640.0f, 360.0f };
		D2D_POINT_2F last2 = { 640.0f, 700.0f };
		d2dData.DrawLine(first, last);
		d2dData.DrawLine(first2, last2);
		//d2dData.FillRect(left, right, top, bottom);
		d2dData.AfterDraw();
		d2dData.Show();
		std::system("pause");
		std::cout << "Now is Direct2d is ready for you draw object" << std::endl;
	}

	return 0;
}
