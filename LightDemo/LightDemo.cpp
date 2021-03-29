#include <iostream>
#include "LightApp.h"

int main()
{
	LightApp lightApp(GetModuleHandle(NULL));

	if (!lightApp.Initialize()))

	return LightApp.Run();
}