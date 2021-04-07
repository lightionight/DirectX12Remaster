
#include <iostream>
#include "lightApp.h"



int main()
{
	RECT r;
	GetWindowRect(GetConsoleWindow(), &r);
	MoveWindow(GetConsoleWindow(), r.left, r.top, 800, 600, TRUE);
	std::cout << "now Start Program" << std::endl;

	LightApp theapp;
	theapp.Initialize();
	std::cout << "Done Initialize " << std::endl;
	return theapp.Run();
	std::cout << "Finish Done" << std::endl;
}

