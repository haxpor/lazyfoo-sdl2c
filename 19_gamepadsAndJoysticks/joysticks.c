/**
 * 19 - Gamepads and Joysticks
 * 
 * Sample 1 demonstrating initialize joystick sub-system.
 * For simple requirement such as here getting axis motion, use joystick is enough.
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
bool setup();
void update();
void handleEvent(SDL_Event *e);
void render();
void close();

// refer to variables declared and defined elsewhere (in common.h)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

#ifndef DISABLE_SDL_TTF_LIB
extern TTF_Font* gFont;
#endif

// global texture for button
LTexture* gTexture = NULL;
// analog joystick's dead zone
const int JOYSTICK_DEAD_ZONE = 8000;
// game joystick 1 handler
SDL_Joystick* gGameJoystick = NULL;
// normalized direction for arrow to render
int xDir = 0;
int yDir = 0;

bool init() {
	// initialize sdl
	// here we also init for joystick sub-system
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
		return false;
	}

	// set texture filtering to linear
	// anisotropic filtering not available in core profile of opengl, need to rely on extension
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		SDL_Log("Warning: Linear texture filtering not enabled!");	
	}

	// check joystick
	if (SDL_NumJoysticks() == 0)
	{
		SDL_Log("Currently, there's no joystick connected");
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
	gTexture = LTexture_LoadFromFile("arrow.png");
	if (gTexture == NULL)
	{
		SDL_Log("Failed to load texture");
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
	// handle joystick - device added
	else if (e->type == SDL_JOYDEVICEADDED)
	{
		// when added it's device index
		SDL_Log("Joystick connected for index %d", e->cdevice.which);

		// load joystick
		gGameJoystick = SDL_JoystickOpen(0);
		if (gGameJoystick == NULL)
		{
			SDL_Log("Warning: Unable to open joystick at index 0 for use. SDL Error: %s", SDL_GetError());
		}
	}
	// handle joystick - device removed
	else if (e->type == SDL_JOYDEVICEREMOVED)
	{
		// when removed, it's instance id
		SDL_Log("Controller disconnected for index %d", e->cdevice.which);

		// close previously openned joystick
		if (gGameJoystick != NULL)
			SDL_JoystickClose(gGameJoystick);
	}
	// handle joystick
	else if (e->type == SDL_JOYAXISMOTION)
	{
		if (gGameJoystick == NULL)
		{
			goto OUT;
		}

		// safer to use instance_id field of SDL_Joystick struct
		// such id is never get reused throughout the lifetime of the application.
		// Thus it never always be 0 for 1st joystick.
		if (e->jaxis.which == SDL_JoystickInstanceID(gGameJoystick))
		{
			// x axis-motion
			if (e->jaxis.axis == 0)
			{
				// left of dead zone
				// x is left
				if (e->jaxis.value < -JOYSTICK_DEAD_ZONE)
				{
					xDir = -1;
				}
				// x is right
				else if (e->jaxis.value > JOYSTICK_DEAD_ZONE)
				{
					xDir = 1;
				}
				else
				{
					xDir = 0;
				}
			}
			// y axis-motion
			else if (e->jaxis.axis == 1)
			{
				// below of dead zone
				// y is up
				if (e->jaxis.value < -JOYSTICK_DEAD_ZONE)
				{
					yDir = -1;
				}
				// above of dead zone
				// y is down
				else if (e->jaxis.value > JOYSTICK_DEAD_ZONE)
				{
					yDir = 1;
				}
				else
				{
					yDir = 0;
				}
			}
		}
// to exit from handle joystick event if game joystick is not connected yet
OUT:;
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

	// calculate angle
	double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);
	// draw arrow based on analog stick of joystick
	LTexture_RenderEx(gTexture, SCREEN_WIDTH/2 - gTexture->width/2, SCREEN_HEIGHT/2 - gTexture->height/2, joystickAngle, NULL, false);
}

void close()
{
	// clear resource of our textures
	if (gTexture != NULL)
		LTexture_Free(gTexture);

	// close joystick
	if (gGameJoystick != NULL)
		SDL_JoystickClose(gGameJoystick);

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

