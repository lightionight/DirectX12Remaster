#include <iostream>
#include "TexApp.h"

int main()
{
    TexApp texapp;
    texapp.Initialize();
    std::cout << "Done INitialize " << std::endl;
    return texapp.Run();
}

