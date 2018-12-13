/**
 * 49 - Mutexes and Conditions
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LWindow.h"
#include "LTexture.h"
#include "LTimer.h"
#include "krr_math.h"

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
void handleEvent(SDL_Event *e, float deltaTime);
void update(float deltaTime);
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

// content's rect to clear color in render loop
SDL_Rect content_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

// splash texture
LTexture* splash_texture = NULL;

// threads
SDL_Thread* producer_thread = NULL;
SDL_Thread* consumer_thread = NULL;

// worker functions
int producer( void* data);
int consumer( void* data);
void produce();
void consume();

//protective mutex
SDL_mutex* buffer_lock = NULL;

// conditions
SDL_cond* cond_can_produce = NULL;
SDL_cond* cond_can_consume = NULL;

// the data
int tshared_data = -1;

int producer( void*data )
{
  SDL_Log("Producer started...");

  // seed to random function
  krr_math_rand_seed_time();

  // produce
  for (int i=0; i<5; i++)
  {
    // wait
    SDL_Delay(krr_math_rand_int(999));

    // produce
    produce();
  }

  SDL_Log("Producer finished!");

  return 0;
}

int consumer( void* data )
{
  SDL_Log("Consumer started...");

  // seed to random function
  krr_math_rand_seed_time();

  // consume
  for (int i=0; i<5; i++)
  {
    // wait
    SDL_Delay(krr_math_rand_int(999));

    // consume
    consume();
  }

  SDL_Log("Consumer finished!");

  return 0;
}

void produce()
{
  // lock
  SDL_LockMutex(buffer_lock);

  // if the buffer is full
  if (tshared_data != -1)
  {
    // wait for buffer to be cleared
    // there's no room for producer to produce and fill the buffer, need to wait
    SDL_Log("Producer encountered full buffer, waiting for consumer to empty buffer...");
    SDL_CondWait(cond_can_produce, buffer_lock);
  }

  // fill and show buffer
  tshared_data = krr_math_rand_int(254);
  SDL_Log("Produced %d", tshared_data);

  // unlock
  SDL_UnlockMutex(buffer_lock);

  // signal consumer
  // tell consumer that buffer is filled and ready to be consumed
  SDL_CondSignal(cond_can_consume);
}

void consume()
{
  // lock
  SDL_LockMutex(buffer_lock);

  // if the buffer is empty
  if (tshared_data == -1)
  {
    // wait for buffer to be filled
    // as there's no data to be consumed, need to wait for producer to produce data first
    SDL_Log("Consumer encountered empty buffer, waiting for producer to fill buffer");
    SDL_CondWait(cond_can_consume, buffer_lock);
  }

  // show and empty buffer
  SDL_Log("Consumed %d", tshared_data);
  tshared_data = -1;

  // unlock
  SDL_UnlockMutex(buffer_lock);

  // signal producer
  // now producer can produce as consumer consumed thus empty the data
  SDL_CondSignal(cond_can_produce);
}

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = LWindow_new("49 - Mutexes and Conditions", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
  if (gWindow == NULL) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    return false;
  }
  // set device indepenent resolution for rendering (so no unproper aspect ratio)
  if (SDL_RenderSetLogicalSize(gWindow->renderer, SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
  {
    SDL_Log("Warning: failed to set logical size of window");
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

  // splash texture
  splash_texture = LTexture_LoadFromFile("splash.png");
  if (splash_texture == NULL)
  {
    SDL_Log("Failed to load splash.png");
    return false;
  }

  // create the mutex
  buffer_lock = SDL_CreateMutex();

  // create conditions
  cond_can_produce = SDL_CreateCond();
  cond_can_consume = SDL_CreateCond();

  // run the threads
  // note: data is the thread name we sent to show console later when those threads are in work
  producer_thread = SDL_CreateThread(producer, "Producer", (void*)"Producer");
  // delay in between creation of thread
  SDL_Delay(krr_math_rand_int2(16, 47));
  consumer_thread = SDL_CreateThread(consumer, "Consumer", (void*)"Consumer");

  return true;
}

void handleEvent(SDL_Event *e, float deltaTime)
{
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
      // 0 for windowed mode
      SDL_SetWindowFullscreen(gWindow->window, 0);
      gWindow->_fullscreen = false;
    }
    else
    {
      // SDL_WINDOW_FULLSCREEN_DESKTOP for "fake" fullscreen without changing videomode
      // depends on type of game, and performance aim i.e. FPS game might want to do "real" fullscreen
      // by changing videomode to get performance gain, but point and click with top-down tile-based
      // might not need to change videomode to match the desire spec.
      //
      // as well this needs to work with SDL_RenderSetLogicalSize() function to make it works.
      SDL_SetWindowFullscreen(gWindow->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
      gWindow->_fullscreen = true;
      gWindow->is_minimized = false;
    }
  }
}

void update(float deltaTime)
{

}

void render(float deltaTime)
{
  if (!gWindow->is_minimized)
  {
    // clear screen (bg)
    SDL_SetRenderDrawColor(gWindow->renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(gWindow->renderer);

    // clear screen (content)
    SDL_SetRenderDrawColor(gWindow->renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderFillRect(gWindow->renderer, &content_rect);

    LTexture_Render(splash_texture, 0 ,0);

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
  // wait for all threads to finish
  SDL_WaitThread(producer_thread, NULL);
  SDL_WaitThread(consumer_thread, NULL);

  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  // destroy mutex
  SDL_DestroyMutex(buffer_lock);
  // destroy conditions
  SDL_DestroyCond(cond_can_produce);
  SDL_DestroyCond(cond_can_consume);

  // destroy texture
  if (splash_texture != NULL)
    LTexture_Free(splash_texture);

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

