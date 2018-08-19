/**
 * 04 - Key Presses
 *
 * Demonstrate key event handling. It will show corresponding image whenever up, down, left, or right key is pressed.
 * If user presses Esc key, the program will quit.
 * 
 * As a plus, this tutorial configured its Makefile to build with -g flag via gcc to produce source level debugging information,
 * so you could test it with 'gdb keypresses'.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// key press surfaces constants
enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

bool init();
bool loadMedia();
void close();

// just a test function calling from gdb (or llvm) for testing purpose
// to print address of current surface
void debuggerPrint();

// load individual image
SDL_Surface* loadSurface(const char* const path);

// the window we'll be rendering to
SDL_Window* gWindow = NULL;

// the surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// The images that correspond to a keypress
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

// current displayed image
SDL_Surface* gCurrentSurface = NULL;

void debuggerPrint()
{
	// add \n as well to let buffer flushed so we could see output in gdb
	printf("address of gCurrentSurface: %p\n", &gCurrentSurface);
	printf("address of variable gCurrentSurface pointing to: %p\n", gCurrentSurface);
}

bool init() {
	// initialize flag
	bool success = true;

	// initialize sdl
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		// create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			// Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}
	return success;
}

SDL_Surface* loadSurface(const char* const path)
{
	// load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP((const char*)path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL error: %s\n", path, SDL_GetError());
	}
	return loadedSurface;
}

bool loadMedia()
{
	// load default surface
	gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] = loadSurface("press.bmp");
	if (gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] == NULL)
	{
		printf("Failed to load default image!\n");
		return false;
	}

	gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] = loadSurface("up.bmp");
	if (gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] == NULL)
	{
		printf("Failed to load up image!\n");
		return false;
	}

	gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] = loadSurface("down.bmp");
	if (gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] == NULL)
	{
		printf("Failed to load down image!\n");
		return false;
	}

	gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] = loadSurface("left.bmp");
	if (gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] == NULL)
	{
		printf("Failed to load left image!\n");
		return false;
	}

	gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] = loadSurface("right.bmp");
	if (gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] == NULL)
	{
		printf("Failed to load right image!\n");
		return false;
	}

	// all images are loaded successfully
	return true;
}

void close()
{
	// deallocate surface
	for (int i=0; i<KEY_PRESS_SURFACE_TOTAL; i++)
	{
		SDL_FreeSurface(gKeyPressSurfaces[i]);
		gKeyPressSurfaces[i] = NULL;
	}

	// destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	// this will be taken care of by SDL, but we set it to NULL as well anyway
	gScreenSurface = NULL;

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

			// set to default surface first
			gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];

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
						// select surface to display based on what key user presses
						switch (e.key.keysym.sym)
						{
							case SDLK_UP:
								gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
								break;
							case SDLK_DOWN:
								gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
								break;
							case SDLK_LEFT:
								gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
								break;
							case SDLK_RIGHT:
								gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
								break;
							case SDLK_ESCAPE:
								quit = true;
								break;
							default:
								gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
								break;
						}
					}
				}

				// apply the image
				SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
				// update the surface
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}

	// free resource and close SDL
	close();

	return 0;
}
