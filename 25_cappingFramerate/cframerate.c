/**
 * 25 - Calculate Frame Rate
 *
 * Improved from original
 *   - offer preprocessor to disable / enable FPS calculation from build Makefile
 *   - support fixed time step game loop
 *   - FPS calculation instead of via LTimer, using delta time in fixed time loop
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTexture.h"
#include "LTimer.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SETFRAME(var, arg1, arg2, arg3, arg4)		\
  do {										\
    var.x = arg1;							\
    var.y = arg2;							\
    var.w = arg3;							\
    var.h = arg4;							\
  } while(0)

// cap thus using fixed deltaTime step
#define TARGET_FPS 60
#define FIXED_DELTATIME 1.0f / TARGET_FPS

// -- functions
bool init();
bool setup();
void update(float deltaTime);
void handleEvent(SDL_Event *e, float deltaTime);
void render(float deltaTime);
void close();

// -- variables
bool quit = false;

// buffer of characters to render
// +1 for null-terminated character
const int BUFFER = 128 + 1;

// independent time loop
Uint32 currTime = 0;
Uint32 prevTime = 0;

// global texture for button
SDL_Color gTextColor = { 0, 0, 0, 255 };
char fpsText[BUFFER];
char frameCountText[BUFFER];

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
  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
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
  // load ttf font
  gFont = TTF_OpenFont("lazy.ttf", 28);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load lazy font! Error: %s", TTF_GetError());
    return false;
  }

  return true;
}

void update(float deltaTime)
{

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

  // form fps text
#ifndef DISABLE_FPS_CALC
  snprintf(fpsText, BUFFER-1, "Average Frames Per Second %.2f", common_avgFPS);
#else
  snprintf(fpsText, BUFFER-1, "Average Frames Per Second (disabled)");
#endif

  // render timertext as texture
  // note that we need to fre fpsTextTexture as well as this is dynamically created a new LTexture
  // every frame.
  LTexture *fpsTextTexture = LTexture_LoadFromRenderedText(fpsText, gTextColor, 0);
  if (fpsTextTexture != NULL)
  {
    LTexture_Render(fpsTextTexture, (SCREEN_WIDTH - fpsTextTexture->width)/2, (SCREEN_HEIGHT - fpsTextTexture->height)/2);
  }
  else
  {
    SDL_Log("Unable to render fps texture");
  }

  // form total frame text
#ifndef DISABLE_FPS_CALC
  snprintf(frameCountText, BUFFER-1, "Total Frame Count %d", common_frameCount);
#else
  snprintf(frameCountText, BUFFER-1, "Total Frame Count (disabled)");
#endif

  LTexture *totalFrameTextTexture = LTexture_LoadFromRenderedText(frameCountText, gTextColor, 0);
  if (totalFrameTextTexture != NULL && fpsTextTexture != NULL)
  {
    LTexture_Render(totalFrameTextTexture, (SCREEN_WIDTH - totalFrameTextTexture->width)/2, (SCREEN_HEIGHT - fpsTextTexture->height)/2 + fpsTextTexture->height + 10);

    free(totalFrameTextTexture);
    totalFrameTextTexture = NULL;
  }
  else
  {
    SDL_Log("Unable to render total frame texture");
  }

  // free fpsTextTexture
  if (fpsTextTexture != NULL)
  {
    free(fpsTextTexture);
    fpsTextTexture = NULL;
  }
}

void close()
{
  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

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
        // prepare delta time to feed to both handleEvent(), update(), and render()
        prevTime = currTime;
        currTime = SDL_GetTicks();
        // calculate per second
        float deltaTime = (currTime - prevTime) / 1000.0f;

        // handle events on queue
        // if it's 0, then it has no pending event
        // we keep polling all event in each game loop until there is no more pending one left
        while (SDL_PollEvent(&e) != 0)
        {
          // update user's handleEvent()
          handleEvent(&e, deltaTime);
        }

#ifndef DISABLE_FPS_CALC
        // fixed step
        common_frameTime += deltaTime;
        common_frameAccumTime += deltaTime;
#endif
        if (common_frameTime >= FIXED_DELTATIME)
        {
#ifndef DISABLE_FPS_CALC
          common_frameCount++;

          // check to reset frame time
          if (common_frameAccumTime >= 1.0f)
          {
            common_avgFPS = common_frameCount / common_frameAccumTime;
            common_frameCount = 0;
            common_frameAccumTime -= 1.0f;
          }
#endif
          common_frameTime = 0.0f;

          update(FIXED_DELTATIME);
          render(FIXED_DELTATIME);
        }
        else {
          render(0); 
        }

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

