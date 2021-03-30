#include <iostream>
#include "LightApp.h"


LRESULT CALLBACK GlobalProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LightApp::GetLightApp()->MsgProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
	LightApp theapp;

	theapp.Initialize();
	return theapp.Run();
}