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

// -- functions
bool init();
bool setup();
void update(float deltaTime);
void handleEvent(SDL_Event *e, float deltaTime);
void render(float deltaTime);
void close();

// -- variables
bool quit = false;

// refer to variables declared and defined elsewhere (in common.h)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

#ifndef DISABLE_SDL_TTF_LIB
extern TTF_Font* gFont;
#endif

// global texture for button
LTexture* gTexture = NULL;

// independent time loop
Uint32 currTime = 0;
Uint32 prevTime = 0;
float frameTime = 0.0f;
float avgFPS = 0.0f;

bool init() {
	// initialize sdl
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
		return false;
	}
	
	// create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
		return false;
	}

	// create renderer for window
	// as we use SDL_Texture, now we need to use renderer to render stuff
	// also use vsync to cap framerate to what video card can do
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL)
	{
		SDL_Log("SDL could not create renderer! SDL_Error: %s", SDL_GetError());
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
		SDL_Log("SDL_Image could not initialize! SDL_image Error: %s", IMG_GetError());
		return false;
	}

#ifndef DISABLE_SDL_TTF_LIB
	// initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
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
		SDL_Log("Failed to load texture");
		return false;
	}

	return true;
}

void update(float deltaTime)
{
	// nothing here for this sample.
}

void handleEvent(SDL_Event *e, float deltaTime)
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
        break;
		}
	}
}

void render(float deltaTime)
{
	// clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(gRenderer);

  frameTime += deltaTime;
  if (frameTime >= 1.0f)
  {
    avgFPS = frameCount;
    // reset framecount
    frameCount = 0;
    // reset frame time
    frameTime -= 1.0f;
  }
  SDL_Log("avgFPS %.2f", avgFPS);
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
		SDL_Log("Failed to initialize");
	}	
	else
	{
		// load media, and set up
		if (!setup())
		{
			SDL_Log("Failed to setup!");
		}
		else
		{
			// event handler
			SDL_Event e;

			// while application is running
			while (!quit)
			{
        // prepare delta time to feed to handleEvent(), update() and render()
        prevTime = currTime;
        currTime = SDL_GetTicks();
        // calculate per second
        float deltaTime = (currTime = prevTime) / 1000.0f;
        
				// handle events on queue
				// if it's 0, then it has no pending event
				// we keep polling all event in each game loop until there is no more pending one left
				while (SDL_PollEvent(&e) != 0)
				{
					// update user's handleEvent()
					handleEvent(&e, deltaTime);
				}

				update(deltaTime);
				render(deltaTime);			

				// update screen from any rendering performed since this previous call
				// as we don't use SDL_Surface now, we can't use SDL_UpdateWindowSurface
				SDL_RenderPresent(gRenderer);

        // increment frame count
        frameCount++;
			}
		}
	}

	// free resource and close SDL
	close();

	return 0;
}

