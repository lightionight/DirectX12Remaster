#include "WinApp.h"

LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// forward to current window class handle it
	return WinApp::GetWinApp()->MsgProc(hwnd, uMsg, wParam, lParam);
}

WinApp::WinApp(HINSTANCE hInstance)
{
	mHINSTCE = hInstance;
	if (mAPP == nullptr)
		mAPP = this;
}

WinApp::~WinApp() { }

WinApp* WinApp::mAPP = nullptr;
WinApp* WinApp::GetWinApp() { return mAPP; }

//-----------------------------Initialize function----------------------------------//

bool WinApp::Initialize(int widowWidth, int windowHeight, LPCWSTR className)
{
	SetWindowRect(widowWidth, windowHeight);
	if (!InitializeWindows(className))
		return false;
	return true;
}

bool WinApp::InitializeWindows(LPCWSTR className)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(LPWNDCLASSEX));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpfnWndProc = GlobalProc;
	wc.hInstance = mHINSTCE;

	if (className == (LPCWSTR)nullptr)
		wc.lpszClassName = mDefaultClassName;
	else
		wc.lpszClassName = className;
	// if register class failed 
	if (FAILED(RegisterClassEx(&wc)))
		MessageBoxEx(nullptr, L"Register Windows Class faided", L"ERROR", NULL, NULL);

	mWindowRect = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&mWindowRect, WS_OVERLAPPEDWINDOW, false);
	int width = mWindowRect.right - mWindowRect.left;
	int height = mWindowRect.bottom - mWindowRect.top;
	if (className == (LPCWSTR)nullptr)
	{
		mHWND = CreateWindow(mDefaultClassName, L"Win32 App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			width, height, 0, 0, mHINSTCE, NULL);
	}
	else
	{
		mHWND = CreateWindow(className, L"Win32 App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			width, height, NULL, NULL, mHINSTCE, NULL);
	}
	
	if (!mHWND)
	{
		MessageBox(NULL, L"Create Windows Faild.", L"ERROR", NULL);
		return false;
	}

	ShowWindow(mHWND, SW_SHOW);
	UpdateWindow(mHWND);

	return true;
}

bool WinApp::InitializeDirectX()
{
	return true;
}

void WinApp::SetWindowRect(int windowWidth, int windowHeight)
{
	mClientWidth = windowWidth;
	mClientHeight = windowHeight;
}

//_____________________________Loop function_________________________________________//
int WinApp::Run()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// if no input handle other task, like deal the last input refresh graphics show
			if (!mPause)
			{
				
				Update();
				Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}

LRESULT WinApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{

		}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WinApp::Update()
{

}

void WinApp::Draw()
{

}