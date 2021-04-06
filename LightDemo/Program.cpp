
#include <iostream>
#include "lightApp.h"

int main()
{
	RECT r;
	GetWindowRect(GetConsoleWindow(), &r);
	MoveWindow(GetConsoleWindow(), r.left, r.top, 1280, 720, TRUE);
	LightApp theapp;
	theapp.Initialize();
	return theapp.Run();
}

