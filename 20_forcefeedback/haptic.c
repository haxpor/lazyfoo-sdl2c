/**
 * 20 - Force feedback (haptic device)
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
// game controller 1 handler
SDL_GameController* gGameController = NULL;
SDL_Haptic* gGameControllerHaptic = NULL;

bool init() {
  // initialize sdl
  // here we also init for controller sub-system
  // this in turn also enable joystick sub-system
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // set texture filtering to linear
  // anisotropic filtering not available in core profile of opengl, need to rely on extension
  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
  {
    SDL_Log("Warning: Linear texture filtering not enabled!");	
  }

  // check number of joysticks connected
  if (SDL_NumJoysticks() == 0)
  {
    SDL_Log("Currently, there's no joystick connected");
  }

  // create window
  gWindow = SDL_CreateWindow("20 - Force Feedback", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
  gTexture = LTexture_LoadFromFile("splash.png");
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
  // handle controller - device added
  else if (e->type == SDL_CONTROLLERDEVICEADDED)
  {
    // when added it's device index
    SDL_Log("Controller connected for index %d", e->cdevice.which);

    // load controller
    gGameController = SDL_GameControllerOpen(0);
    if (gGameController == NULL)
    {
      SDL_Log("Warning: Unable to open controller at index 0 for use. SDL Error: %s", SDL_GetError());
    }
    else
    {
      // OK
      // get haptic device
      gGameControllerHaptic = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(gGameController));
      if (gGameControllerHaptic == NULL)
      {
        SDL_Log("Warning: Unable to get haptic device from game controller 0: %s", SDL_GetError());
      }
      else
      {
        // OK
        // initialize haptic rumble
        if (SDL_HapticRumbleInit(gGameControllerHaptic) < 0)
        {
          SDL_Log("Warning: Unable to initialize haptic rumble: %s", SDL_GetError());
        }
      }
    }
  }
  // handle controller - device removed
  else if (e->type == SDL_CONTROLLERDEVICEREMOVED)
  {
    // when removed, it's instance id
    SDL_Log("Controller disconnected for index %d", e->cdevice.which);

    // close previously openned controller
    if (gGameController != NULL)
      SDL_GameControllerClose(gGameController);
  }
  // handle controller - button down
  else if (e->type == SDL_CONTROLLERBUTTONDOWN && gGameController != NULL && gGameControllerHaptic != NULL)
  {
    // if press on button A then we rumble
    if (e->cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gGameController)) &&
        e->cbutton.button == SDL_CONTROLLER_BUTTON_A)
    {
      // play rumble at 75% for 500 ms
      if (SDL_HapticRumblePlay(gGameControllerHaptic, 0.75, 500) < 0)
      {
        SDL_Log("Warning: Cannot play rumble: %s", SDL_GetError());
      }
    }
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

  // draw splash on screen
  LTexture_Render(gTexture, 0, 0);
}

void close()
{
  // clear resource of our textures
  if (gTexture != NULL)
  {
    LTexture_Free(gTexture);
    gTexture = NULL;
  }

  // close haptic
  if (gGameControllerHaptic != NULL)
  {
    SDL_HapticClose(gGameControllerHaptic);
    gGameControllerHaptic = NULL;
  }
  // close controller
  if (gGameController != NULL)
  {
    SDL_GameControllerClose(gGameController);
    gGameController = NULL;
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

