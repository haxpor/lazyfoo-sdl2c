/**
 * 11 - Clip rendering and Sprite sheets
 *
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTexture.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

bool init();
bool loadMedia();
void close();

// refer to variables declared and defined elsewhere (in common.h)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

// our textures
LTexture gSpriteSheetTexture;
SDL_Rect gSpriteClips[4];

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
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
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
	
	return true;
}

bool loadMedia()
{
	// load texture
	if (!LTexture_LoadFromFile("dots.png", &gSpriteSheetTexture))
	{
		printf("Failed to load texture!\n");
		return false;
	}

	// set clips for each sprite in the sheet
	// each single sprite in the sheet has size of 100x100
	gSpriteClips[0].x = 0;
	gSpriteClips[0].y = 0;
	gSpriteClips[0].w = 100;
	gSpriteClips[0].h = 100;

	gSpriteClips[1].x = 100;
	gSpriteClips[1].y = 0;
	gSpriteClips[1].w = 100;
	gSpriteClips[1].h = 100;

	gSpriteClips[2].x = 0;
	gSpriteClips[2].y = 100;
	gSpriteClips[2].w = 100;
	gSpriteClips[2].h = 100;

	gSpriteClips[3].x = 100;
	gSpriteClips[3].y = 100;
	gSpriteClips[3].w = 100;
	gSpriteClips[3].h = 100;

	return true;
}

void close()
{
	// clear resource of our textures
	// if gSpriteSheetTexture is allocated on heap via malloc() then
	// we have to manually free() after the following call as well.
	LTexture_FreeInternal(&gSpriteSheetTexture);

	// destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// quit SDL_image
	IMG_Quit();

	// quit sdl subsystem
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
		// load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			// main loop flag
			bool quit = false;
			
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
					// user requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					// user presses a key
					else if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym)
						{
							case SDLK_ESCAPE:
								quit = true;
								break;
						}
					}
				}

				// clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
				SDL_RenderClear(gRenderer);

				// render top left sprite
				LTexture_ClippedRender(&gSpriteSheetTexture, 0, 0, &gSpriteClips[0]);
				// render top right sprite
				LTexture_ClippedRender(&gSpriteSheetTexture, SCREEN_WIDTH-gSpriteClips[1].w, 0, &gSpriteClips[1]);
				// render bottom left sprite
				LTexture_ClippedRender(&gSpriteSheetTexture, 0, SCREEN_HEIGHT-gSpriteClips[2].h, &gSpriteClips[2]);
				// render bottom right sprite
				LTexture_ClippedRender(&gSpriteSheetTexture, SCREEN_WIDTH-gSpriteClips[3].w, SCREEN_HEIGHT-gSpriteClips[3].h, &gSpriteClips[3]);

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
