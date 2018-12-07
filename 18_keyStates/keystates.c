/**
 * 18 - Key events
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
bool loadMedia();
void close();

// refer to variables declared and defined elsewhere (in common.h)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

#ifndef DISABLE_SDL_TTF_LIB
extern TTF_Font* gFont;
#endif

// global texture for button
LTexture* gCurrentTextureToShow = NULL;
LTexture* gInfoTexture = NULL;
LTexture* gUpTexture = NULL;
LTexture* gDownTexture = NULL;
LTexture* gLeftTexture = NULL;
LTexture* gRightTexture = NULL;

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  // create window
  gWindow = SDL_CreateWindow("18 - Key States", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (gWindow == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  // create renderer for window
  // as we use SDL_Texture, now we need to use renderer to render stuff
  // also use vsync to cap framerate to what video card can do
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

#ifndef DISABLE_SDL_TTF_LIB
  // initialize SDL_ttf
  if (TTF_Init() == -1)
  {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }
#endif

  return true;
}

// include any asset loading sequence, and preparation code here
bool setup()
{
  // info texture
  gInfoTexture = LTexture_LoadFromFile("press.png");
  if (gInfoTexture == NULL)
  {
    SDL_Log("Failed to load press.png texture");
    return false;
  }

  // up texture
  gUpTexture = LTexture_LoadFromFile("up.png");
  if (gUpTexture == NULL)
  {
    SDL_Log("Failed to load up.png texture");
    return false;
  }

  // down texture
  gDownTexture = LTexture_LoadFromFile("down.png");
  if (gDownTexture == NULL)
  {
    SDL_Log("Failed to load down.png texture");
    return false;
  }

  // left texture
  gLeftTexture = LTexture_LoadFromFile("left.png");
  if (gLeftTexture == NULL)
  {
    SDL_Log("Failed to load left.png texture");
    return false;
  }

  // right texture
  gRightTexture = LTexture_LoadFromFile("right.png");
  if (gRightTexture == NULL)
  {
    SDL_Log("Failed to load right.png texture");
    return false;
  }

  // set logical designated design resolution in case when
  // we try to go for independent resolution later when going in full screen
  SDL_RenderSetLogicalSize(gRenderer, 640, 480);

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
  // user presses a key
  else if (e->type == SDL_KEYDOWN)
  {
    switch (e->key.keysym.sym)
    {
      case SDLK_ESCAPE:
        quit = true;
    }
  }

  // handle key states (key states will be updated after SDL_PollEvent is called prior outside of this function)
  // set texture to show based on key states
  // note: states emitted as scan code
  const Uint8* states = SDL_GetKeyboardState(NULL);
  if (states[SDL_SCANCODE_UP])
  {
    gCurrentTextureToShow = gUpTexture;
  }
  else if (states[SDL_SCANCODE_DOWN])
  {
    gCurrentTextureToShow = gDownTexture;
  }
  else if (states[SDL_SCANCODE_LEFT])
  {
    gCurrentTextureToShow = gLeftTexture;
  }
  else if (states[SDL_SCANCODE_RIGHT])
  {
    gCurrentTextureToShow = gRightTexture;
  }
  // otherwise shwo info texture
  else if (gCurrentTextureToShow != gInfoTexture)
  {
    gCurrentTextureToShow = gInfoTexture;
  }
}

void render()
{
  // clear screen
  SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
  SDL_RenderClear(gRenderer);

  // render current texture on screen
  LTexture_Render(gCurrentTextureToShow, 0, 0);
}

void close()
{
  // clear resource of our textures
  if (gInfoTexture != NULL)
    LTexture_Free(gInfoTexture);
  if (gUpTexture != NULL)
    LTexture_Free(gUpTexture);
  if (gDownTexture != NULL)
    LTexture_Free(gDownTexture);
  if (gLeftTexture != NULL)
    LTexture_Free(gLeftTexture);
  if (gRightTexture != NULL)
    LTexture_Free(gRightTexture);

  gCurrentTextureToShow = NULL;

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
    printf("Failed to initialize\n");
  }	
  else
  {
    // load media, and set up
    if (!setup())
    {
      printf("Failed to setup!\n");
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
