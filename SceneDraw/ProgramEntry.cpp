#include "SceneApp.h"


int WINAPI
wWinMain(
	HINSTANCE hInstace,
	HINSTANCE preHinstace,
	PWSTR nCmdLine,
	int nCmdShow
)
{
	SceneApp theApp(hInstace);
	if (!theApp.Initialize())
		return false;
	return theApp.Run();
}
