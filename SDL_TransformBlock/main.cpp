#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window* g_Window = NULL;
SDL_Surface* g_pDisplaySurface = NULL;
SDL_Surface* g_pBitSurface = NULL;
SDL_Event g_Event;
SDL_Rect g_SrcRect, g_DstRect, g_ClipRect;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	g_Window = SDL_CreateWindow("SDL Transfer Block", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (g_Window == NULL)
		fprintf(stderr, "Create Window Error.\n");
	g_pDisplaySurface = SDL_GetWindowSurface(g_Window);

	g_pBitSurface = SDL_LoadBMP("lena.bmp");
	g_SrcRect.w = g_DstRect.w = g_pBitSurface->w;
	g_SrcRect.h = g_DstRect.h = g_pBitSurface->h;
	g_SrcRect.x = g_SrcRect.y = 0;
	g_ClipRect.x = 32;
	g_ClipRect.y = 32;
	g_ClipRect.w = SCREEN_WIDTH - 64;
	g_ClipRect.h = SCREEN_HEIGHT - 64;
	SDL_SetClipRect(g_pDisplaySurface, &g_ClipRect);
	for (;;)
	{
		if (SDL_PollEvent(&g_Event) == 0)
		{
			g_DstRect.x = rand() % (SCREEN_WIDTH - g_DstRect.w);
			g_DstRect.y = rand() % (SCREEN_HEIGHT - g_DstRect.h);
			//SDL_SetColorKey()
			SDL_BlitSurface(g_pBitSurface, &g_SrcRect, g_pDisplaySurface, &g_DstRect);
			SDL_UpdateWindowSurface(g_Window);
		}
		else
		{
			if (g_Event.type == SDL_QUIT)
				break;
		}
	}
	
	return 0;
}