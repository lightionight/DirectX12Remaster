#include "stdafx.h"

#include "Game.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	Game game;
	game.Init();
	game.Run();

	printf("GameOver");
	return 0;
}