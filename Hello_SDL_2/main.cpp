
#include <SDL2\SDL.h>
#include <stdlib.h>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Surface* g_Display = NULL;
SDL_Window* g_Window = NULL;
SDL_Event g_Event;
SDL_Rect g_Rect;
Uint8 g_R, g_G, g_B;
Uint32 g_Color;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);

	g_Rect.x = 0;
	g_Rect.y = 0;

	g_Rect.w = 640;
	g_Rect.h = 480;

	g_Window = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
	
	g_Display = SDL_CreateRGBSurface()

	SDL_ShowWindow(g_Window);

	SDL_FillRect(g_Display, )

}
