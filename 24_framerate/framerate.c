/**
 * 24 - Calculate Frame Rate
 *
 * Changes from original
 *  - improve to use independent time loop via deltaTime based on second, and not use LTimer
 *  - offer 2 ways to calculate fps either via deltaTime or LTimer
 *  - render total frameCount on screen
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

// set to 1 to calculate fps via delta time (independent time - game loop)
// set to 0 to calculate fps via LTimer
// we set this in Makefile, use -DCALCULATE_WITH_DELTATIME=1 or -DCALCULATE_WITH_DELTATIME=0

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

// buffer of characters to render
// +1 for null-terminated character
const int BUFFER = 128 + 1;

// independent time loop
Uint32 currTime = 0;
Uint32 prevTime = 0;

// global texture for button
SDL_Color gTextColor = { 0, 0, 0, 255 };
int frameCount = 0;
char fpsText[BUFFER];
char frameCountText[BUFFER];

#if CALCULATE_WITH_DELTATIME == 1
float frameTime = 0;
#endif
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

#if CALCULATE_WITH_DELTATIME == 1
  frameTime += deltaTime;
  if (frameTime >= 1.0f)
  {
    avgFPS = frameCount;
    // reset framecount
    frameCount = 0;
    // reset frame time
    frameTime -= 1.0f;
  }
#else
  Uint32 currTime = SDL_GetTicks();
  avgFPS = frameCount / (currTime / 1000.0f);
#endif

  // form fps text
  snprintf(fpsText, BUFFER-1, "Average Frames Per Second %.2f", avgFPS);

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
  snprintf(frameCountText, BUFFER-1, "Total Frame Count %d", frameCount);

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

        update(deltaTime);
        render(deltaTime);			

        // update screen from any rendering performed since this previous call
        // as we don't use SDL_Surface now, we can't use SDL_UpdateWindowSurface
        SDL_RenderPresent(gRenderer);

        // increment frame count
        // note: you can even reset (or cycling) the value of frameCount to zero to not make it overflow
        // but due to calculation taking ideally 1ms to increment 1 frame, then it would take ~24 days
        // to overflow int data type, yep, the game would less likely to be opened for 24 days straight
        frameCount++;
      }
    }
  }

  // free resource and close SDL
  close();

  return 0;
}

