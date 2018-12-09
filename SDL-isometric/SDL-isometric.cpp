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
static float r = 0.0f;
static SDL_Window *window = NULL;
static SDL_GLContext gl_context;


//void render()
//{
//	SDL_GL_MakeCurrent(window, gl_context);
//
//	r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//
//	glClearColor(r, 0.4f, 0.1f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	SDL_GL_SwapWindow(window);
//
//} //render


int SDLCALL watch(void *userdata, SDL_Event* event) 
{
	if (event->type == SDL_APP_WILLENTERBACKGROUND) {
		quitting = true;
	}

	return 1;
}

int main(int argc, char *argv[]) 
{
	const std::string window_title("ICanHasSFML (2.0RC) Window?");
	unsigned int screen_width = 800;
	unsigned int screen_height = 600;
	unsigned int bits_per_pixel = 32;

	unsigned int numDrawn = 0;
	unsigned int numConsidered = 0;

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

	CGameApp gameApp(window);

	//CSpriteSheet spr_sheet("./data/maps/tiles/map_tiles.png", 64, 48);

	//const sf::Input& Input = App.GetInput();

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

		// Set render color to red ( background will be rendered in this color )
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		// Clear winow
		SDL_RenderClear(renderer);

		numDrawn = 0;
		numConsidered = 0;

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
