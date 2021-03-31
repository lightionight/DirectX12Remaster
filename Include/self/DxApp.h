
#ifndef DX_APP_H
#define DX_APP_H

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
	virtual void Update(GameTimer& gt) = 0;
	virtual void Draw(GameTimer& gt) = 0;
	virtual void OnResize();

	void CalculateFrameStates();

	virtual void OnMouseDown(WPARAM btnState, int x, int y) { };
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { };
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { };


public:
	static DXApp* mLightApp;
protected:
	std::unique_ptr<WinDesc> mWinDesc;
	std::unique_ptr<WindowData> mWindow;
	std::unique_ptr<DxData> mDirectX;
	std::unique_ptr<DxDesc> mDXDesc;
	std::unique_ptr<DxBind> mDxBind;
	std::unique_ptr<InputHandle> mInputHandle;

	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;
	GameTimer mTimer;

};


#endif // !DX_APP_H


