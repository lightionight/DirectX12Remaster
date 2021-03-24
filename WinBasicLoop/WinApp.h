#ifndef WIN_APP_H
#define WIN_APP_H

#include "WindowInclude.h"

class WinApp
{
public:
	// Constructor
	WinApp(HINSTANCE hInstance);
	WinApp(const WinApp& rhs) = delete;
	WinApp& operator=(const WinApp& rhs) = delete;
	~WinApp();
public:
	static WinApp* GetWinApp();
	//-----------------------initialized function--------------------------------------------//
	bool Initialize(int windowWidth, int windowHeight, LPCWSTR className = nullptr);
	//------------------------Main Loop Function---------------------------------------------//
	void virtual Update();  // Can override by inhert class 
	void virtual Draw();  //  can override by inhert class
	//------------------------Parameter function -------------------------------------------//
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();
protected:
	void SetWindowRect(int windowWidth, int windowHeight);
private:
	bool InitializeWindows(LPCWSTR className);
	bool InitializeDirectX();
	bool InitializeDirectXContext();


public:
protected:
	LPCWSTR mDefaultClassName = L"WinApp";  // need to set by subClass or inhert default value.
	HINSTANCE mHINSTCE = nullptr;
	HWND      mHWND = nullptr;
	static WinApp* mAPP;
private:
	int mClientWidth = 1920;
	int mClientHeight = 1080;
	RECT mWindowRect = { 0 };
	bool mPause = false;
	bool mMiniized = false;
	bool mMaxnized = false;
};


#endif // !WIN_APP_H

