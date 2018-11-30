/**
 * 30 - Scrolling
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTexture.h"
#include "LTimer.h"
#include "bound_sys.h"
#include "camera.h"

#define LEVEL_WIDTH 1280
#define LEVEL_HEIGHT 960

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

#define DOT_SPEED 200

BoundSystem bound_system;
LTexture *dotTexture = NULL;
LTexture *bgTexture = NULL;
Dot dot;
camera cam;

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
  // load font
  gFont = TTF_OpenFont("../8bitwondor.ttf", 16);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load 8bitwonder.ttf font: %s", TTF_GetError());
    return false;
  }

  // load bg texture
  bgTexture = LTexture_LoadFromFile("bg.png");
  if (bgTexture == NULL)
  {
    SDL_Log("Failed to load bg.png texture: %s", SDL_GetError());
    return false;
  }

  // load dot texture
  dotTexture = LTexture_LoadFromFileWithColorKey("dot.bmp", 0xff, 0xff, 0xff);
  if (dotTexture == NULL)
  {
    SDL_Log("Failed to load dot.bmp: %s", SDL_GetError());
    return false;
  }

  // camera
  // set it at the same position of Dot itself
  camera_init(&cam, 50, 120, SCREEN_WIDTH, SCREEN_HEIGHT);
  cam.lerp_factor = 0.20;

  // initialize Dot
  Dot_Init(&dot, 50, 120, dotTexture);

  // init bound system once
  BoundSystem_source_init(LEVEL_WIDTH, LEVEL_HEIGHT);
  // set global function for bounding to our local bound system
  bound_system.f = BoundSystem_global_screenbound_Dot;

  return true;
}

void update(float deltaTime)
{
  Dot_Update(&dot, deltaTime);
  bound_system.f(&dot);

  // update target position of camera to follow (after updated position of Dot)
  cam.target_x = (dot.posX + dot.collider.r) - SCREEN_WIDTH/2;
  cam.target_y = (dot.posY + dot.collider.r) - SCREEN_HEIGHT/2;

  camera_update_lerpcenter(&cam);
  // bound camera
  if (BoundSystem_bound_camera(&cam))
  {
    // if camera is out of bound, then reposition the dot immediately
  }

  SDL_Log("dot position: %f, %f", dot.posX, dot.posY);
  SDL_Log("cam position: %d, %d", cam.view_rect.x, cam.view_rect.y);
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

  // dot control
  if (e->type == SDL_KEYDOWN && e->key.repeat == 0)
  {
    switch (e->key.keysym.sym)
    {
      case SDLK_UP:
        dot.targetVelY -= DOT_SPEED * deltaTime;
        break;
      case SDLK_DOWN:
        dot.targetVelY += DOT_SPEED * deltaTime;
        break;
      case SDLK_LEFT:
        dot.targetVelX -= DOT_SPEED * deltaTime;
        break;
      case SDLK_RIGHT:
        dot.targetVelX += DOT_SPEED * deltaTime;
        break;
    }
  }
  else if (e->type == SDL_KEYUP && e->key.repeat == 0)
  {
    switch (e->key.keysym.sym)
    {
      case SDLK_UP:
        dot.targetVelY += DOT_SPEED * deltaTime;
        break;
      case SDLK_DOWN:
        dot.targetVelY -= DOT_SPEED * deltaTime;
        break;
      case SDLK_LEFT:
        dot.targetVelX += DOT_SPEED * deltaTime;
        break;
      case SDLK_RIGHT:
        dot.targetVelX -= DOT_SPEED * deltaTime;
        break;
    }
  }
}

void render(float deltaTime)
{
  // clear screen
  SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
  SDL_RenderClear(gRenderer);

#ifndef DISABLE_FPS_CALC
  // render fps on the top right corner
  snprintf(fpsText, FPS_BUFFER-1, "%d", (int)common_avgFPS);

  // generate fps texture
  SDL_Color color = {30, 30, 30, 255};
  LTexture *fpsTexture = LTexture_LoadFromRenderedText(fpsText, color, 0);
  if (fpsTexture != NULL)
  {
    LTexture_Render(fpsTexture, SCREEN_WIDTH - fpsTexture->width - 5, 0);

    free(fpsTexture);
    fpsTexture = NULL;
  }
#endif

  LTexture_Render(bgTexture, -cam.view_rect.x, -cam.view_rect.y);
  Dot_Render_w_camera(&dot, cam.view_rect.x, cam.view_rect.y);
}

void close()
{
  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  if (dotTexture != NULL)
    LTexture_Free(dotTexture);
  if (bgTexture != NULL)
    LTexture_Free(bgTexture);

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
        SDL_RenderPresent(gRenderer);
      }
    }
  }

  // free resource and close SDL
  close();

  return 0;
}

