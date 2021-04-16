#pragma once

#include <SDL2\SDL.h>
#include <SDL2_image\SDL_image.h>
#include <SDL2\SDL_render.h>
#include <SDL2\SDL_timer.h>
#include <SDL2\SDL_config_windows.h>

class Game
{
public:
	Game();
	~Game();
public:
	void Init(const char* title = NULL, int width = NULL, int height = NULL, SDL_bool defaultWindow = SDL_TRUE);
	void Run();
	void Render();
	void Update();
	void Exit();
public:
private:
	SDL_Window* m_SDL_Window = NULL;
	SDL_Renderer* m_Renderer = NULL;
	SDL_Surface* m_SDL_DisplaySurface = NULL;
	SDL_Surface* m_SDL_BitSurface = NULL;
	SDL_Texture* m_SDL_Texture;
	SDL_Event m_Event;
	SDL_AddTimer

	const char* m_Imagpath = "lena.tiff";
	const char* m_title = "SDL 2 Basic";
	int m_ScreenWidth = 1280;
	int m_ScreenHeight = 720;
	SDL_Rect m_WindowRect, m_RenderRect, m_DrawRect;
	SDL_Color m_RenderColor;


};

