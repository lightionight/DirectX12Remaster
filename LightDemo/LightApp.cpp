#include "LightApp.h"
#include <iostream>
#include <windowsx.h>

LightApp::LightApp()
{
	
	
}

LightApp::~LightApp() { }

bool LightApp::Initialize()
{
	if (mLightApp != nullptr)
		mLightApp = this;
	mWindow = std::make_unique<WindowData>();
	mDesc = std::make_unique<DirectDesc>();
	mDirectX = std::make_unique<DirectData>();
	mInputHandle = std::make_unique<InputHandle>();

	mWindow->Initialize(L"LightApp", L"LightDemo", 1280, 720, GlobalProc);
	mInputHandle->Initialize(mWindow->ClientWidth, mWindow->ClientHeight, true);

	mDesc->Initialize();
	m4xMsaaQuality =  mDirectX->CheckFeatureSupport(mDesc->BackBufferFormat);
	InitializeDesc();
	mDirectX->Initialize(*mDesc.get());
	// For Inhert Desin
	subInitlize();

	return true;
}

void LightApp::InitializeDesc()
{
	mDesc->SwapChainDesc->BufferDesc.Width = mWindow->ClientWidth;
	mDesc->SwapChainDesc->BufferDesc.Height = mWindow->ClientHeight;
	mDesc->SwapChainDesc->SampleDesc.Count = m4xMsaaState ? 4 : 1;
	mDesc->SwapChainDesc->SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	mDesc->SwapChainDesc->BufferCount = mDirectX->SwapChainBufferCount;
	mDesc->SwapChainDesc->OutputWindow = mWindow->Hwnd;
	mDesc->RtvHeapDesc->NumDescriptors = mDirectX->SwapChainBufferCount;
}

int LightApp::Run()
{
	MSG msg = { 0 };
	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		mInputHandle->Manager->Update();  // Update input Manager.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();
			if (!mWinStatu.Paused)
			{
				CalculateFrameStates();
				// Handle Input
				if (mInputHandle->Map->GetBoolWasDown(Button::MOUSE_L || Button::MOUSE_R))
				{
					std::cout << "Mouse Button is Press" << std::endl;
				}

				// Update Scene
				Update(mTimer);
				Draw(mTimer);

			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}
	
LRESULT LightApp::MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mWinStatu.Paused = true;
			mTimer.Stop();
		}
		else
		{
			mWinStatu.Paused = false;
			mTimer.Start();
		}
		return 0;

	case WM_SIZE:
		mWindow->ClientWidth = LOWORD(lParam);
		mWindow->ClientHeight = HIWORD(lParam);
		if (mDirectX->Device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mWinStatu.Paused = true;
				mWinStatu.Minimized = true;
				mWinStatu.Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mWinStatu.Paused = false;
				mWinStatu.Maximized = true;
				mWinStatu.Minimized = false;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (mWinStatu.Minimized)
				{
					mWinStatu.Paused = false;
					mWinStatu.Minimized = false;
					OnResize();
				}
				else if (mWinStatu.Maximized)
				{
					mWinStatu.Paused = false;
					mWinStatu.Maximized = false;
					OnResize();
				}
				else if (mWinStatu.Resizing)
				{

				}
				else
				{
					OnResize();
				}
			}
		}
		return 0;
		
	case WM_ENTERSIZEMOVE:
		mWinStatu.Paused = true;
		mWinStatu.Resizing = true;
		mTimer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		mWinStatu.Paused = false;
		mWinStatu.Resizing = false;
		mTimer.Start();
		OnResize();
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2)
			Set4xMsaaState(!m4xMsaaState);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void LightApp::OnResize()
{
	mDesc->ResizeDesc(mWindow->ClientWidth, mWindow->ClientHeight, m4xMsaaState, m4xMsaaQuality);
	mDirectX->ResizeWindow(*mDesc.get(), mWindow->ClientWidth, mWindow->ClientHeight);
}

