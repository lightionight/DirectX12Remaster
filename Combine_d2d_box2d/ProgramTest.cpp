#include <self\Win32Context.h>
#include <self\Dx2dData.h>
#include <self\D2dDrawObject.h>

#include <iostream>


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
    D2dDrawObject geo(&d2dData,D2dDrawObject::D2dDrawObjectType::Geometry);

    D2D1_POINT_2F pointList[] =  {
        D2D1::Point2F(0.0f, 0.0f),
        D2D1::Point2F(10.0f, 10.0f),
        D2D1::Point2F(30.0f, 30.0f),
        D2D1::Point2F(500.0f, 500.0f),
        D2D1::Point2F(1000.0f, 700.0f),
        D2D1::Point2F(800.0f, 0.0f)
    };

	geo.Add(pointList, 6);

	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != WM_QUIT)
	{
		d2dData.PerpareDraw(D2D1::ColorF::Red);
		geo.Draw(&d2dData);
		d2dData.DrawLine(pointList[3], pointList[5]);
		d2dData.AfterDraw();
	}

	return 0;
}
