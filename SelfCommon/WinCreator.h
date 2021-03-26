#ifndef WIN_CREATOR_H
#define WIN_CREATOR_H

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include "Win32Include.h"

class WinCreator
{
public:
	WinCreator(const LPCWSTR className, const LPCWSTR windowsName, int width, int height);
	~WinCreator();

public:
    static WinCreator* winCreator;
	
protected:
	HISNTACE GetHinstace() { return mHisntace; };

private:
	HINSTACE mHinstace;
	HWND   mHwnd;
	int mClientWidth;
	int mClientHeight;
};

WinCreator::winCreator = nullptr;

#endif // !WINCREATOR_H
