
#include "DxApp.h"
#include <windowsx.h>
#include <iostream>

DXApp* DXApp::mLightApp = nullptr;

DXApp* DXApp::GetLightApp() { return mLightApp; }

DXApp::DXApp()
{

}

DXApp::~DXApp()
{
	mWindow.release();
	mWinDesc.release();
	mDirectX.release();
	mInputHandle.release();
	mDXDesc.release();
}

void DXApp::Initialize()
{
	if (mLightApp == nullptr)
		mLightApp = this;
	mWindow      = std::make_unique<WindowData>();
	mDXDesc      = std::make_unique<DxDesc>();
	mDirectX     = std::make_unique<DxData>();
	mInputHandle = std::make_unique<InputHandle>();
	mWinDesc     = std::make_unique<WinDesc>();
	mDxBind      = std::make_unique<DxBind>();  // After reSetCommandList This Guy Need ReSetting.

	// Win32 Init
	mWinDesc->Initialize(L"LightApp", L"LightDemo", 1280, 720, GlobalProc);
	mWindow->Initialize(mWinDesc.get());
	mInputHandle->Initialize(mWinDesc->ClientWidth, mWinDesc->ClientHeight, true);

	// DirectX Init
	m4xMsaaQuality =  mDirectX->CheckFeatureSupport(mDXDesc->BackBufferFormat);
	mDXDesc->Initialize(mWinDesc.get(), mDirectX.get(), mWindow.get(), m4xMsaaState, m4xMsaaQuality);
	mDirectX->Initialize(mDXDesc.get());
	mDxBind->Initialize(mDirectX.get());
	OnResize();
}

int DXApp::Run()
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
			if (!mWinDesc->Paused)
			{
				CalculateFrameStates();
				// Handle Input
				InputDetect();
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
	
LRESULT DXApp::MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mWinDesc->Paused = true;
			mTimer.Stop();
		}
		else
		{
			mWinDesc->Paused = false;
			mTimer.Start();
		}
		return 0;

	case WM_SIZE:
		mWinDesc->ClientWidth = LOWORD(lParam);
		mWinDesc->ClientHeight = HIWORD(lParam);
		if (mDirectX->SwapChain)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mWinDesc->Paused = true;
				mWinDesc->Minimized = true;
				mWinDesc->Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mWinDesc->Paused = false;
				mWinDesc->Minimized = false;
				mWinDesc->Maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (mWinDesc->Minimized)
				{
					mWinDesc->Paused = false;
					mWinDesc->Minimized = false;
					OnResize();
				}
				else if (mWinDesc->Maximized)
				{
					mWinDesc->Paused = false;
					mWinDesc->Maximized = false;
					OnResize();
				}
				else if (mWinDesc->Resizing)
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
		mWinDesc->Paused = true;
		mWinDesc->Resizing = true;
		mTimer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		mWinDesc->Paused = false;
		mWinDesc->Resizing = false;
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

void DXApp::OnResize()
{
	mDXDesc->ResizeDesc(mWinDesc->ClientWidth, mWinDesc->ClientHeight, m4xMsaaState, m4xMsaaQuality);
	mDirectX->ResizeWindow(mDXDesc.get(), mWinDesc->ClientWidth, mWinDesc->ClientHeight);
}

void DXApp::CalculateFrameStates()
{

}

LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DXApp::GetLightApp()->MsgProc(hwnd, uMsg, wParam, lParam);
}

void DXApp::InputDetect()
{
	// if button have press
	if (mInputHandle->Map->GetBoolWasDown(MouseL | MouseR))
	{
		ReleaseCapture();
	}
	    
	if(mInputHandle->Map->GetBool(MouseL))
	{
		OnMouseMove((int)mInputHandle->Map->GetFloat(MouseX), (int)mInputHandle->Map->GetFloat(MouseY));
		//SetCapture(mWindow->Hwnd);
	    OnRotate((int)mInputHandle->Map->GetFloat(MouseX), (int)mInputHandle->Map->GetFloat(MouseY));
	}
	else if(mInputHandle->Map->GetBool(MouseR))
	{
		OnMouseMove((int)mInputHandle->Map->GetFloat(MouseX), (int)mInputHandle->Map->GetFloat(MouseY));
		//SetCapture(mWindow->Hwnd);
	    OnScale((int)mInputHandle->Map->GetFloat(MouseX), (int)mInputHandle->Map->GetFloat(MouseY));
	}
	else
	{
		
	}
}


