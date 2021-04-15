
#include <SDL2\SDL.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		fprintf(stderr, "Could not init SDL.\n");
	else
	{
		fprintf(stdout, "SDL init Done! \n");
		SDL_Quit();
	}
	return 0;
}