#include "stdafx.h"
#include "Game.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>

Game::Game()
{

}

Game::~Game()
{

}

void Game::Init(const char* title, int width, int height, SDL_bool defaultWindow)
{
	Uint32 initFlag = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
	SDL_Init(initFlag);

	if (width == NULL)
	{
		m_WindowRect.w = m_ScreenWidth;
		m_WindowRect.h = m_ScreenHeight;
	}
	else
	{
		m_WindowRect.w = m_ScreenWidth = width;
		m_WindowRect.h = m_ScreenHeight = height;
	}

	m_WindowRect.x = SDL_WINDOWPOS_UNDEFINED;
	m_WindowRect.y = SDL_WINDOWPOS_UNDEFINED;
	


	if (defaultWindow)
	{
		m_SDL_Window = SDL_CreateWindow(m_title, m_WindowRect.x, m_WindowRect.y, m_WindowRect.w, m_WindowRect.h, SDL_WINDOW_SHOWN);
		for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
		{
			SDL_RendererInfo rendererInfo = { };
			SDL_GetRenderDriverInfo(i, &rendererInfo);
			printf(rendererInfo.name);
			printf("\n");
			
			if (rendererInfo.name == "direct3d")
			{
				continue;
			}
			m_Renderer = SDL_CreateRenderer(m_SDL_Window, i, SDL_RENDERER_ACCELERATED);
			SDL_RenderGetViewport(m_Renderer, &m_RenderRect);
			if (m_Renderer == NULL)
				printf("Create Dx11 Error");
			break;
		}
	}
}


void Game::Update()
{
	m_RenderColor.r = rand() % 256;
	m_RenderColor.g = rand() % 256;
	m_RenderColor.b = rand() % 256;
	m_RenderColor.a = rand() % 256;

	m_DrawRect.x = rand() % (m_RenderRect.w - 150);
	m_DrawRect.y = rand() % (m_RenderRect.h - 150);
	m_DrawRect.w = (rand() + 50) % (m_WindowRect.w);
	m_DrawRect.h = (rand() + 50) % (m_WindowRect.h);

}

void Game::Render()
{

	SDL_SetRenderDrawColor(m_Renderer, m_RenderColor.r, m_RenderColor.g, m_RenderColor.b, m_RenderColor.a);

	SDL_Delay(500);
	SDL_RenderFillRect(m_Renderer, &m_DrawRect);

	SDL_RenderPresent(m_Renderer);
}

void Game::Run()
{
	for (;;)
	{
		if(SDL_PollEvent(&m_Event) == NULL)
		{
			Update();
			Render();
		}
	    else
	    {
	        if (m_Event.type == SDL_QUIT)
	        {
		        Exit();
				break;
	        }
	    }
	}
}





void Game::Exit()
{
	SDL_Quit();
}