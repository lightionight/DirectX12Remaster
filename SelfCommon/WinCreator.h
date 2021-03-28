#ifndef WIN_CREATOR_H
#define WIN_CREATOR_H

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include "Win32Include.h"

struct WindowsData
{
	HINSTANCE Hinstace;
	HWND Hwnd;
};




LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class WinCreator
{
public:
	WinCreator(const LPCWSTR className, const LPCWSTR windowsName, int width, int height);
	~WinCreator();

public:
    static WinCreator* winCreator;
	
protected:
	HINSTANCE GetHinstace() { return mHinstace; };

private:
	
	HINSTANCE mHinstace;
	HWND   mHwnd;
	int mClientWidth;
	int mClientHeight;
};

WinCreator* WinCreator::winCreator = nullptr;

#endif // !WINCREATOR_H
