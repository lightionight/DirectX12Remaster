
#ifndef DX_APP_H
#define DX_APP_H

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "Dx12Context.h"
#include "Win32Context.h"
#include "Inputhandle.h"
#include <dx12book\GameTimer.h>

LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class DXApp
{
public:
	DXApp();
	~DXApp();
public:
	static DXApp* GetLightApp();
	virtual void Initialize();
	int Run();

	bool Get4xMsaaState()const { return m4xMsaaState; }
	void Set4xMsaaState(bool value) { m4xMsaaState = value; }

	virtual LRESULT CALLBACK MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;
	virtual void OnResize();

	void CalculateFrameStates();
	void InputDetect();

	virtual void OnRotate(int x, int y) = 0;
	virtual void OnScale(int x, int y) = 0;
	//virtual void OnPaddle(int x, int y)  = 0;
	virtual void OnMouseMove(int x, int y) = 0;




public:
	static DXApp* mLightApp;
protected:
	std::unique_ptr<WinDesc> mWinDesc;
	std::unique_ptr<WindowData> mWindow;
	std::unique_ptr<DxData> mDirectX;
	std::unique_ptr<DxDesc> mDXDesc;
	std::unique_ptr<DxBind> mDxBind;
	// not to handle input in virtual class left behind in sub class to deal with.
	std::unique_ptr<InputHandle> mInputHandle;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;
	GameTimer mTimer;

};


#endif // !DX_APP_H


