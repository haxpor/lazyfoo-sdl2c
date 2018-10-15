/**
 * template main source file for sample
 *
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTexture.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SETFRAME(var, arg1, arg2, arg3, arg4)		\
		do {										\
			var.x = arg1;							\
			var.y = arg2;							\
			var.w = arg3;							\
			var.h = arg4;							\
		} while(0)

bool quit = false;

bool init();
bool loadMedia();
void close();

// refer to variables declared and defined elsewhere (in common.h)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

#ifndef DISABLE_SDL_TTF_LIB
extern TTF_Font* gFont;
#endif

// global texture for button
LTexture* gTexture = NULL;

bool init() {
	// initialize sdl
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	// create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// create renderer for window
	// as we use SDL_Texture, now we need to use renderer to render stuff
	// also use vsync to cap framerate to what video card can do
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL)
	{
		printf("SDL could not create renderer! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	// initialize png loading
	// see https://www.libsdl.org/projects/SDL_image/docs/SDL_image.html#SEC8
	// returned from IMG_Init is all flags initted, so we could check for all possible
	// flags we aim for
	int imgFlags = IMG_INIT_PNG;
	int inittedFlags = IMG_Init(imgFlags);
	if ( (inittedFlags & imgFlags) != imgFlags)
	{
		// from document, not always that error string from IMG_GetError() will be set
		// so don't depend on it, just for pure information
		printf("SDL_Image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

#ifndef DISABLE_SDL_TTF_LIB
	// initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}
#endif
	
	return true;
}

// include any asset loading sequence, and preparation code here
bool setup()
{
	// load buttons texture
	gTexture = LTexture_LoadFromFile("your-texture.png");
	if (gTexture == NULL)
	{
		printf("Failed to load texture\n");
		return false;
	}

	return true;
}

void update()
{
	// nothing here for this sample.
}

void handleEvent(SDL_Event *e)
{
	// user requests quit
	if (e->type == SDL_QUIT)
	{
		quit = true;
	}
	// user presses a key
	else if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_ESCAPE:
			quit = true;
		}
	}
}

void render()
{
	// clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(gRenderer);
}

void close()
{
	// clear resource of our textures
	if (gTexture != NULL)
		LTexture_Free(gTexture);

	// destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

#ifndef DISABLE_SDL_TTF_LIB
	TTF_Quit();
#endif

	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	// start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize\n");
	}	
	else
	{
		// load media, and set up
		if (!setup())
		{
			printf("Failed to setup!\n");
		}
		else
		{
			// event handler
			SDL_Event e;

			// while application is running
			while (!quit)
			{
				// handle events on queue
				// if it's 0, then it has no pending event
				// we keep polling all event in each game loop until there is no more pending one left
				while (SDL_PollEvent(&e) != 0)
				{
					// update user's handleEvent()
					handleEvent(&e);
				}

				update();
				render();			

				// update screen from any rendering performed since this previous call
				// as we don't use SDL_Surface now, we can't use SDL_UpdateWindowSurface
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	// free resource and close SDL
	close();

	return 0;
}

