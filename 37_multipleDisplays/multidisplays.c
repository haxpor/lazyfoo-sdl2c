/**
 * 37 - Multiple Displays
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LWindow.h"
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

// independent time loop
Uint32 currTime = 0;
Uint32 prevTime = 0;

#ifndef DISABLE_FPS_CALC
#define FPS_BUFFER 7+1
char fpsText[FPS_BUFFER];
#endif

// display data
int total_displays = 0;
SDL_Rect* displaybounds = NULL;

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = LWindow_new("37 - Multiple Displays", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
  if (gWindow == NULL) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
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
  // load font
  gFont = TTF_OpenFont("../Minecraft.ttf", 16);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load Minecraft.ttf font: %s", TTF_GetError());
    return false;
  }

  // get total displays
  total_displays = SDL_GetNumVideoDisplays();
  // check error for getting total displays
  if (total_displays < 0)
  {
    SDL_Log("Failed to get number of video display: %s", SDL_GetError());
    return false;
  }
  // provide warning if total number of displays is less than 2
  if (total_displays < 2)
  {
    SDL_Log("Warning: only one display connected.");
  }

  // get bounds of display
  displaybounds = malloc(sizeof(SDL_Rect)*total_displays);
  for (int i=0; i<total_displays; i++)
  {
    SDL_GetDisplayBounds(i, &displaybounds[i]);
  }

  return true;
}

void update(float deltaTime)
{

}

void handleEvent(SDL_Event *e, float deltaTime)
{
  LWindow_handle_event(gWindow, e, deltaTime);

  // user requests quit
  if (e->type == SDL_QUIT ||
      (e->key.keysym.sym == SDLK_ESCAPE))
  {
    quit = true;
  }
  // toggle fullscreen via enter key
  else if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN)
  {
    if (gWindow->_fullscreen)
    {
      SDL_SetWindowFullscreen(gWindow->window, SDL_FALSE);
      gWindow->_fullscreen = false;
    }
    else
    {
      SDL_SetWindowFullscreen(gWindow->window, SDL_TRUE);
      gWindow->_fullscreen = true;
      gWindow->is_minimized = false;
    }
  }
  // cycle up through display list
  else if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_UP && total_displays >= 2)
  {
    ++gWindow->display_id;
    // cyclic bound value
    if (gWindow->display_id > total_displays-1)
    {
      gWindow->display_id = 0;
    }

    // move window to center of next target display
    SDL_SetWindowPosition(gWindow->window, displaybounds[gWindow->display_id].x + (displaybounds[gWindow->display_id].w/2 - gWindow->width/2),
        displaybounds[gWindow->display_id].y + (displaybounds[gWindow->display_id].h/2 - gWindow->height/2));
  }
  // cycle down through display list
  else if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_DOWN && total_displays >= 2)
  {
    --gWindow->display_id;
    // cyclic bound value
    if (gWindow->display_id < 0)
    {
      gWindow->display_id = total_displays - 1;
    }

    // move window to center of next target display
    SDL_SetWindowPosition(gWindow->window, displaybounds[gWindow->display_id].x + (displaybounds[gWindow->display_id].w/2 - gWindow->width/2),
        displaybounds[gWindow->display_id].y + (displaybounds[gWindow->display_id].h/2 - gWindow->height/2));
  }
}

void render(float deltaTime)
{
  if (!gWindow->is_minimized)
  {
    // clear screen
    SDL_SetRenderDrawColor(gWindow->renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(gWindow->renderer);

#ifndef DISABLE_FPS_CALC
    // render fps on the top right corner
    snprintf(fpsText, FPS_BUFFER-1, "%d", (int)common_avgFPS);

    // generate fps texture
    SDL_Color color = {30, 30, 30, 255};
    LTexture *fpsTexture = LTexture_LoadFromRenderedText(fpsText, color, 0);
    if (fpsTexture != NULL)
    {
      LTexture_Render(fpsTexture, SCREEN_WIDTH - fpsTexture->width - 5, 10);
      LTexture_Free(fpsTexture);
    }
#endif
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

  // free displaybounds
  if (total_displays > 0 && displaybounds != NULL)
  {
    free(displaybounds);
    displaybounds = NULL;
  }

  // destroy window
  LWindow_free(gWindow);

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

          // handle events on queue
          // if it's 0, then it has no pending event
          // we keep polling all event in each game loop until there is no more pending one left
          while (SDL_PollEvent(&e) != 0)
          {
            // update user's handleEvent()
            handleEvent(&e, FIXED_DELTATIME);
          }

          update(FIXED_DELTATIME);
          render(FIXED_DELTATIME);
        }
        else {
          render(0); 
        }

        // update screen from any rendering performed since this previous call
        // as we don't use SDL_Surface now, we can't use SDL_UpdateWindowSurface
        SDL_RenderPresent(gWindow->renderer);
      }
    }
  }

  // free resource and close SDL
  close();

  return 0;
}

