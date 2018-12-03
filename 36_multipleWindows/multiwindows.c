/**
 * 36 - Multiple windows
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LWindow.h"
#include "LTexture.h"
#include "LTimer.h"

int screen_width = 640;
int screen_height = 480;
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

// define additional windows we will open; apart from main one gWindow
#define ADDITIONAL_WINDOWS 2
LWindow windows[ADDITIONAL_WINDOWS];

static void render_others_1(float delta_time);
static void render_others_2(float delta_time);

// callback when window has resized its size
// this callback can handle logic for all windows as we have parameter 'window_id'.
void on_window_resize_forall(Uint32 window_id, int new_width, int new_height)
{
  // only if it's main window
  if (window_id == gWindow->id)
  {
    screen_width = new_width;
    screen_height = new_height;

    SDL_Log("Window resized");   
  }
}

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create main window
  gWindow = LWindow_new("Main Window", screen_width, screen_height, SDL_WINDOW_RESIZABLE, 0);
  if (gWindow == NULL) {
    SDL_Log("Main window could not be created! SDL_Error: %s", SDL_GetError());
    return false;
  }
  // listen to window resize event
  gWindow->on_window_resize = on_window_resize_forall;

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

  if (!LWindow_init(&windows[0], "Window 2", screen_width, screen_height, SDL_WINDOW_RESIZABLE, 0))
  {
    SDL_Log("Failed to initialize 2nd window");
    return false;
  }
  if (!LWindow_init(&windows[1], "Window 3", screen_width, screen_height, SDL_WINDOW_RESIZABLE, 0))
  {
    SDL_Log("Failed to initialize 3rd window");
    return false;
  }

  // set render function
  windows[0].render = render_others_1;
  windows[1].render = render_others_2;

  return true;
}

void update(float deltaTime)
{
  bool all_windows_closed = true;

  if (gWindow->is_shown)
    all_windows_closed = false;
  
  // keep checking if main window is still shown
  for (int i=0; i<ADDITIONAL_WINDOWS && gWindow->is_shown; i++)
  {
    if (windows[i].is_shown)
    {
      all_windows_closed = false;
      break;
    }
  }

  // if all windows are closed, then quit the program
  if (all_windows_closed)
  { 
    quit = true;
  }
}

void handleEvent(SDL_Event *e, float deltaTime)
{
  // handle event for both main window, and additional windows
  LWindow_handle_event(gWindow, e, deltaTime);
  for (int i=0; i<ADDITIONAL_WINDOWS; i++)
  {
    LWindow_handle_event(&windows[i], e, deltaTime);
  }

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
      case SDLK_1:
        LWindow_focus(gWindow);
        break;
      case SDLK_2:
        LWindow_focus(&windows[0]);
        break;
      case SDLK_3:
        LWindow_focus(&windows[1]);
        break;
    }
  }
}

void render(float deltaTime)
{
  if (!gWindow->is_minimized)
  {
    // clear screen
    SDL_SetRenderDrawColor(gWindow->renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(gWindow->renderer);

    // note: for multiple windows, showing fps is not totally accruate as
    // it needs to take care rendering content for all windows, not just one
    // so such fps number is total for all windows but show in main window only
#ifndef DISABLE_FPS_CALC
    // render fps on the top right corner
    snprintf(fpsText, FPS_BUFFER-1, "%d", (int)common_avgFPS);

    // generate fps texture
    SDL_Color color = {30, 30, 30, 255};
    LTexture *fpsTexture = LTexture_LoadFromRenderedText(fpsText, color, 0);
    if (fpsTexture != NULL)
    {
      LTexture_Render(fpsTexture, screen_width - fpsTexture->width - 5, 10);
      LTexture_Free(fpsTexture);
    }
#endif
  }
}

/// render function for additional first window
void render_others_1(float delta_time)
{
  if (!windows[0].is_minimized)
  {
    // clear screen in red
    SDL_SetRenderDrawColor(windows[0].renderer, 0xff, 0, 0, 0xff);
    SDL_RenderClear(windows[0].renderer);
  }
}

void render_others_2(float delta_time)
{
  if (!windows[1].is_minimized)
  {
    // clear screen in blue
    SDL_SetRenderDrawColor(windows[1].renderer, 0, 0, 0xff, 0xff);
    SDL_RenderClear(windows[1].renderer);
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

  // free main window
  LWindow_free(gWindow);
  // free additional windows
  for (int i=0; i<ADDITIONAL_WINDOWS; i++)
  {
    LWindow_free_internals(&windows[i]);
  }

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

          // render additional windows
          for (int i=0; i<ADDITIONAL_WINDOWS; i++)
          {
            windows[i].render(FIXED_DELTATIME);
          }
        }
        else {
          render(0); 

          // render for additional windows
          for (int i=0; i<ADDITIONAL_WINDOWS; i++)
          {
            windows[i].render(0);
          }
        }

        // update screen from any rendering performed since this previous call
        // as we don't use SDL_Surface now, we can't use SDL_UpdateWindowSurface
        SDL_RenderPresent(gWindow->renderer);

        // update screen for additional windows
        for (int i=0; i<ADDITIONAL_WINDOWS; i++)
        {
          SDL_RenderPresent(windows[i].renderer);
        }
      }
    }
  }

  // free resource and close SDL
  close();

  return 0;
}

