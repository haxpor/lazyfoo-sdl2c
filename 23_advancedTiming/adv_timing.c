/**
 * 23 - Advanced Timers
 *
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

// -- functions
bool init();
bool setup();
void update();
void handleEvent(SDL_Event *e);
void render();
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
const int BUFFER = 512 + 1;

// global texture for button
LTexture* gPromptTexture = NULL;
LTexture* gPromptTexture2 = NULL;
SDL_Color gTextColor = { 0, 0, 0, 255 };
LTimer* timer = NULL;
char timerText[BUFFER];

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = SDL_CreateWindow("23 - Advanced Timers", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
  // load ttf font
  gFont = TTF_OpenFont("lazy.ttf", 28);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load lazy font! Error: %s", TTF_GetError());
    return false;
  }

  // load prompt textures
  // 1st line prompt texture
  gPromptTexture = LTexture_LoadFromRenderedText("Press S to Start or Stop the Timer", gTextColor, 0);
  if (gPromptTexture == NULL)
  {
    SDL_Log("Unable to render prompt texture!");
    return false;
  }

  // 2nd line prompt texture
  gPromptTexture2 = LTexture_LoadFromRenderedText("Press P to Pause or Resume the Timer", gTextColor, 0);
  if (gPromptTexture2 == NULL)
  {
    SDL_Log("Unable to render prompt texture 2!");
    return false;
  }

  // setup timer
  timer = LTimer_CreateNew();

  return true;
}

void update()
{
  // get current time (ticks) from timer
  snprintf(timerText, BUFFER-1, "Seconds since start time %.2f", LTimer_GetTicks(timer) / 1000.0f);
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
        break;

      case SDLK_s:
        if (timer->started)
        {
          LTimer_Stop(timer);
        }
        else
        {
          LTimer_Start(timer);
        }
        break;

      case SDLK_p:
        if (timer->started && timer->paused)
        {
          LTimer_Resume(timer);
        }
        else if (timer->started && !timer->paused)
        {
          LTimer_Pause(timer);
        }
        break;
    }
  }
}

void render()
{
  // clear screen
  SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
  SDL_RenderClear(gRenderer);

  // render timertext as texture
  // note that we need to fre timerTexture as well as this is dynamically created a new LTexture
  // every frame.
  LTexture *timerTexture = LTexture_LoadFromRenderedText(timerText, gTextColor, 0);
  if (timerTexture != NULL)
  {
    LTexture_Render(gPromptTexture, (SCREEN_WIDTH - gPromptTexture->width)/2, 0);
    LTexture_Render(gPromptTexture2, (SCREEN_WIDTH - gPromptTexture2->width)/2, gPromptTexture->height + 5);
    LTexture_Render(timerTexture, (SCREEN_WIDTH - timerTexture->width)/2, (SCREEN_HEIGHT - timerTexture->height)/2);

    free(timerTexture);
    timerTexture = NULL;
  }
  else
  {
    SDL_Log("Unable to render time texture");
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

  // free texture
  if (gPromptTexture != NULL)
  {
    LTexture_Free(gPromptTexture);
  }
  if (gPromptTexture2 != NULL)
  {
    LTexture_Free(gPromptTexture2);
  }

  // free timer
  if (timer != NULL)
  {
    LTimer_Free(timer);
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

