//***************************************************************************************
// BoxApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//
// Shows how to draw a box in Direct3D 12.
//
// Controls:
//   Hold the left mouse button down and move the mouse to rotate.
//   Hold the right mouse button down and move the mouse to zoom in and out.
//***************************************************************************************

#include "BoxApp.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PWSTR cmdLine, int showCmd)
{
    BoxApp theApp(hInstance);
    if (!theApp.Initialize())
        return 0;
    return theApp.Run();
}