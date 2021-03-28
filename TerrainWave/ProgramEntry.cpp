#include "TerrainApp.h"

int WINAPI wWinMain(
	HINSTANCE hInstace,
	HINSTANCE preHInstance,
	PWSTR nCmdLine,
	int nCmdShow)
{
	TerrainApp theApp(hInstace);
	if (!theApp.Initialize())
		return 0;
	return theApp.Run();


}