// SDL-isometric.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <SDL.h>
#include <SDL_opengl.h>

#include <stdlib.h> //rand()

#include <string>
#include <vector>

//#if defined(_WIN32)
//#include <windows.h>
//extern int main(int argc, char* argv[]);
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
//{
//	return main(__argc, __argv);
//}
//#endif // _WIN32

//#include "common.h"

#include "SpriteSheet.h"
#include "TiledMap.h"

#include "gameapp.h"
#include "MacrosAndWrappers.h"

static bool quitting = false;
static SDL_Window *window = NULL;
static SDL_GLContext gl_context;


int SDLCALL watch(void *userdata, SDL_Event* event) 
{
	if (event->type == SDL_APP_WILLENTERBACKGROUND) {
		quitting = true;
	}

	return 1;
}

int main(int argc, char *argv[]) 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_OPENGL);
	gl_context = SDL_GL_CreateContext(window);

	// Setup renderer
	SDL_Renderer* renderer = NULL;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_AddEventWatch(watch, NULL);

	CGameApp gameApp(window, renderer);

	// Start game loop
	bool Running = true;
	while (Running)
	{
		// Process events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			// Close window : exit
			if (event.type == SDL_QUIT) {
				Running = false;
			}

			// Escape key : exit
			if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE))
				Running = false;

			// handle game events
			gameApp.handleEvent(event);
		}

		// Set the active window before using OpenGL commands
		// It's useless here because active window is always the same,
		// but don't forget it if you use multiple windows or controls

		// background will be rendered in this color
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

		// Clear window
		SDL_RenderClear(renderer);

		gameApp.update();
		gameApp.render();

		// Finally, display rendered frame on screen
		SDL_RenderPresent(renderer);
	}

	SDL_DelEventWatch(watch, NULL);
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
} //main
