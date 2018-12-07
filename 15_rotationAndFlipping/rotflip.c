/**
 * 15 - Rotation and Flipping
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

// our textures and animation
const int kWalkingAnimationFrames = 4;
SDL_Rect gAnimationFrames[kWalkingAnimationFrames];
LTexture gTexture;

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  // create window
  gWindow = SDL_CreateWindow("15 - Rotation and Flipping", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (gWindow == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  // create renderer for window
  // as we use SDL_Texture, now we need to use renderer to render stuff
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

  return true;
}

bool loadMedia()
{
  // load texture
  if (!LTexture_LoadFromFile("arrow.png", &gTexture))
  {
    printf("Failed to load arrow texture!\n");
    return false;
  }

  return true;
}

void close()
{
  // clear resource of our textures
  // if gTexture is allocated on heap via malloc() then
  // we have to manually free() after the following call as well.
  LTexture_Free(&gTexture);

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

      // angle of rotation of texture
      double degrees = 0;
      // flip type
      SDL_RendererFlip flipType = SDL_FLIP_NONE;

      // current frame
      int frame = 0;

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
              case SDLK_a:
                degrees -= 60;
                break;
              case SDLK_d:
                degrees += 60;
                break;
              case SDLK_q:
                flipType = SDL_FLIP_HORIZONTAL;
                break;
              case SDLK_w:
                flipType = SDL_FLIP_NONE;
                break;
              case SDLK_e:
                flipType = SDL_FLIP_VERTICAL;
                break;
            }
          }
        }

        // clear screen
        SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(gRenderer);

        // render arrow
        // NULL for center means at the center of destination rectangle to draw
        LTexture_RenderEx(&gTexture, (SCREEN_WIDTH - gTexture.width) / 2, (SCREEN_HEIGHT - gTexture.height) / 2, degrees, NULL, flipType);

        // update screen from any rendering performed since this previous call
        // as we don't use SDL_Surface now, we can't use SDL_UpdateWindowSurface
        SDL_RenderPresent(gRenderer);

        // go to next frame
        ++frame;

        // cycle animation
        if (frame / 4 >= kWalkingAnimationFrames)
        {
          frame = 0;
        }
      }
    }
  }

  // free resource and close SDL
  close();

  return 0;
}
