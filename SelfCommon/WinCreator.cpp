#include "WinCreator.h"
#include "stdInclude.h"
WinCreator::WinCreator(const LPCWSTR className, const LPCWSTR windowName, int width, int height)
{
	
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC,  GlobalProc, NULL,
		NULL ,GetModuleHandle(NULL), NULL, NULL, NULL, NULL, className, NULL };
	if (FAILED(RegisterClassEx(&wc)))
	{
		std::cout << "Register Class Failed in windows Creator app Line 9" << std::endl;
	}


}