/**
 * 02 - Helloworld
 *
 * A basic building block of code for working on in future tutorials. It loads .bmp image, blit into created window's surface, then wait
 * for 5 seconds until it will quit itself.
 *
 * Notice that during waiting (delay), the window's titlebar and window itself isn't responsive.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

bool init();
bool loadMedia();
void close();

// the window we'll be rendering to
SDL_Window* gWindow = NULL;

// the surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

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

bool loadMedia()
{
  // loading success flag
  bool success = true;

  // load splash image
  gHelloWorld = SDL_LoadBMP("hello_world.bmp");
  if (gHelloWorld == NULL)
  {
    printf("Unable to load image %s! SDL Error: %s\n", "hello_world.bmp", SDL_GetError());
    success = false;
  }
  return success;
}

void close()
{
  // deallocate surface
  SDL_FreeSurface(gHelloWorld);
  gHelloWorld = NULL;

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
      // apply the image
      SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
      // update the surface
      SDL_UpdateWindowSurface(gWindow);
      // wait two seconds before the app will quit
      SDL_Delay(5000);
    }
  }

  // free resource and close SDL
  close();

  return 0;
}
