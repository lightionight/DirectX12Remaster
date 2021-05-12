
#include <windowsx.h>
#include <Windows.h>
#include <iostream>
#include <self\Win32Context.h>
#include <self\Dx2dData.h>
#include <self\D2dDrawObject.h>

// Using For Box2d Module Test.
#include <self\Box2dSceneManager.h>

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
	
	Box2dSceneManager scene;

	winDesc.Initialize(L"Hello Direct2D", L"First Taste For Direct2D", 1280, 720, GlobalProc); // Need Add MESSAGE HANDLE FUNCTION
	winData.Initialize(&winDesc);

	D2dData d2dData(winData.Hwnd);
	D2dDrawObject geo(&d2dData, D2dDrawObject::D2dDrawObjectType::Geometry);
	scene.Initialize(-9.8f, 0.0f, -20.0f, 10.0f, 20.0f);

    b2Vec2 pointsArr[4] {b2Vec2(10.0f, 10.0f), b2Vec2(10.0f, 20.0f), b2Vec2(20.0f, 20.0f), b2Vec2(0.0f, 20.0f)};
	scene.AddBox2dObjects("box", pointsArr, 4, b2BodyType::b2_dynamicBody);
	Box2dObject* obj = scene.GetSceneObject("box");
	geo.Add(obj); // There has problem, need to fix
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != WM_QUIT)
	{
		scene.Run();
		geo.Update(obj->GetCurrentPosition());
		d2dData.PerpareDraw(D2D1::ColorF::Red);
		geo.Draw(&d2dData);
		d2dData.AfterDraw();
		//obj = scene.GetSceneObject("box");
		std::system("pause");
		//std::cout << "Now is Direct2d is ready for you draw object" << std::endl;
		//std::cout << "X : " << obj->GetCurrentPosition().x << "Y : " << obj->GetCurrentPosition().y << std::endl;
	}

	return 0;
}