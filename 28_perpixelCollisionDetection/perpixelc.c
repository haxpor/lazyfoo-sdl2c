/**
 * 28 - Per-Pixel Collision Detection
 *
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTexture.h"
#include "LTimer.h"
#include "Dot.h"

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

// speed per second
#define DOT_SPEED 200
#define LERP_FACTOR 0.07

// demonstrate Dot's API that can work with both stack or heap variables
// for heap, you need to Dot_Free() to free internal memory and free() for your variable
// anyway for Dot's API right now, there's no Dot_Free() but you get the idea
Dot dotA;
Dot* dotB = NULL;
LTexture *dotTexture = NULL;
SDL_Rect wall;

// screenbounder system
typedef struct {
  bool (*f)(Dot*);  
} BoundSystem;
BoundSystem boundSystem;
int screenWidth;
int screenHeight;

// bound Dot against screen
// prior to this call screenWidth and screenHeight need to be set to current window size
bool screenBoundDot(Dot* dot)
{
  bool collided = false;

  if (dot->posX < 0 || dot->posX + dot->texture->width > screenWidth)
  {
    // move back
    dot->posX -= dot->velX;
    Dot_ShiftColliders(dot);

    collided = true;
  }

  if (dot->posY < 0 || dot->posY + dot->texture->height > screenHeight)
  {
    // move back
    dot->posY -= dot->velY;
    Dot_ShiftColliders(dot);

    collided = true;
  }

  return collided;
}

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = SDL_CreateWindow("28 - Per-pixel Collision Detection", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

  // load dot texture
  dotTexture = LTexture_LoadFromFileWithColorKey("dot.bmp", 0xFF, 0xFF, 0xFF);
  if (dotTexture == NULL)
  {
    SDL_Log("Failed to load dot.bmp: %s", SDL_GetError());
    return false;
  }

  // initialize dot
  Dot_Init(&dotA, 50, 120, dotTexture);
  dotB = malloc(sizeof(Dot));
  Dot_Init(dotB, 120, 120, dotTexture);

  // wall
  wall.x = 300;
  wall.y = 40;
  wall.w = 40;
  wall.h = 400;

  // get window size
  SDL_GetWindowSize(gWindow, &screenWidth, &screenHeight);
  // setup bound system
  boundSystem.f = screenBoundDot;

  return true;
}

void update(float deltaTime)
{
  // update position of itself
  Dot_Update(&dotA, deltaTime);
  // update collision checking against dotB
  Dot_UpdateCollisions(&dotA, dotB->colliders, 11, &dotB->roughCollider);
  // update collision checking against wall
  Dot_UpdateCollision(&dotA, &wall);

  // bound dotA against screen
  boundSystem.f(&dotA);

  Dot_Update(dotB, deltaTime);
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
        dotA.targetVelY -= DOT_SPEED * deltaTime;
        break;
      case SDLK_DOWN:
        dotA.targetVelY += DOT_SPEED * deltaTime;
        break;
      case SDLK_LEFT:
        dotA.targetVelX -= DOT_SPEED * deltaTime;
        break;
      case SDLK_RIGHT:
        dotA.targetVelX += DOT_SPEED * deltaTime;
        break;
    }
  }
  else if (e->type == SDL_KEYUP && e->key.repeat == 0)
  {
    switch (e->key.keysym.sym)
    {
      case SDLK_UP:
        dotA.targetVelY += DOT_SPEED * deltaTime;
        break;
      case SDLK_DOWN:
        dotA.targetVelY -= DOT_SPEED * deltaTime;
        break;
      case SDLK_LEFT:
        dotA.targetVelX += DOT_SPEED * deltaTime;
        break;
      case SDLK_RIGHT:
        dotA.targetVelX -= DOT_SPEED * deltaTime;
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
    LTexture_Free(fpsTexture);
    fpsTexture = NULL;
  }
#endif

  // render wall
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderDrawRect(gRenderer, &wall);

  // render dots
  Dot_Render(&dotA);
  Dot_Render(dotB);
}

void close()
{
  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  // free dot texture
  if (dotTexture != NULL)
  {
    LTexture_Free(dotTexture);
  }

  // free dotB
  free(dotB);
  dotB = NULL;

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

