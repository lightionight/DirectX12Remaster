#ifndef LIGHT_APP_H
#define LIGHT_APP_H


#include <d3dApp.h>
#include "Data.h"
#include "Inputhandle.h"

LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class LightApp
{
public:
	LightApp();
	~LightApp();
public:
	bool Initialize();
	int Run();

	static LightApp* GetLightApp();
	bool Get4xMsaaState()const { return m4xMsaaState; }
	void Set4xMsaaState(bool value) { m4xMsaaState = value; }


	virtual LRESULT CALLBACK MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void InitializeDesc();
	virtual void subInitlize();

	virtual void Update(GameTimer& gt);
	virtual void Draw(GameTimer& gt);
	virtual void OnResize();

	void CalculateFrameStates();

	virtual void OnMouseDown(WPARAM btnState, int x, int y) { };
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { };
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { };


public:
	static LightApp* mLightApp;
protected:
	std::unique_ptr<WindowData> mWindow;
	std::unique_ptr<DirectData> mDirectX;
	std::unique_ptr<DirectDesc> mDesc;
	std::unique_ptr<InputHandle> mInputHandle;
	WindowStatu mWinStatu;
	bool m4xMsaaState = false;
	UINT m4xMsaaQuality = 0;
	GameTimer mTimer;

};


#endif // !LIGHT_APP_H


