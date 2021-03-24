#include "WinApp.h"

int WINAPI
wWinMain(HINSTANCE hInstace, HINSTANCE preHinstance, PWSTR pCmdLine, int nCmdShow)
{
	WinApp theApp(hInstace);
	if (!theApp.Initialize(800, 600))
	{
		MessageBoxEx(NULL, L"Window Class Initialize error.", L"ERROR", NULL, NULL);
		return NULL;
	}
	return theApp.Run();
}