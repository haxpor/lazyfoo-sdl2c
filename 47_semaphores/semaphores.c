/**
 * 47 - Semaphores
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
// hinttext
LTexture* hinttext_texture = NULL;
// worker thread function
static int worker(void *data);
// semaphore (actually it's mutex as backend, check SDL_sysmutex.c)
SDL_sem* sem_datalock = NULL;
int tshared_data = -1;
SDL_Thread* thread_a = NULL;
SDL_Thread* thread_b = NULL;

int worker(void *data)
{
  SDL_Log("%s Starting ...", data);
  
  // seed random value for random function per thread
  // seed random function in krr_math
  krr_math_rand_seed_time();

  // work 5 times
  for (int i=0; i<5; i++)
  {
    // wait randomly
    // random between 16-47 (inclusive on both end)
    SDL_Delay(krr_math_rand_int2(16, 47));

    // lock
    // note: what's inside SDL_SemWait/SDL_SemPost is critical section code
    SDL_SemWait(sem_datalock);

    // print pre work data
    SDL_Log("%s gets %d", data, tshared_data);

    // do some work
    tshared_data = krr_math_rand_int(255);

    // print post work data
    SDL_Log("%s sets %d [%s:%d/%d]", data, tshared_data, data, i+1, 5);

    // unlock
    SDL_SemPost(sem_datalock);

    // wait randomly
    SDL_Delay(krr_math_rand_int2(16, 655));
  }

  SDL_Log("%s finished!", data);

  return 0;
}

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = LWindow_new("47 - Semaphores", SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
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
    SDL_Log("Failed to create texture splash.png");
    return false;
  }

  // hinttext texture
  SDL_Color text_color = { 0x00, 0x00, 0x00, 0xff };
  hinttext_texture = LTexture_LoadFromRenderedText("Note: If hit ESCAPE, program will wait until all threads done their work.", text_color, 0);
  if (hinttext_texture == NULL)
  {
    SDL_Log("Failed to create texture for hint text");
    return false;
  }

  // create semaphore
  sem_datalock = SDL_CreateSemaphore(1);

  // seed time for random function, delay time between each thread's startup
  krr_math_rand_seed_time();

  // run the threads
  // note: data is the thread name we sent to show console later when those threads are in work
  thread_a = SDL_CreateThread(worker, "Thread A", (void*)"Thread A");
  // delay in between creation of thread
  SDL_Delay(krr_math_rand_int2(16, 47));
  thread_b = SDL_CreateThread(worker, "Thread B", (void*)"Thread B");

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

    LTexture_Render(splash_texture, 0, 0);
    LTexture_Render(hinttext_texture, 20, SCREEN_HEIGHT - 5 - hinttext_texture->height);

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
  SDL_WaitThread(thread_a, NULL);
  SDL_WaitThread(thread_b, NULL);

  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  // splash texture
  if (splash_texture != NULL)
    LTexture_Free(splash_texture);
  // hinttext
  if (hinttext_texture != NULL)
    LTexture_Free(hinttext_texture);

  // free semaphores
  // note: at remarks section of https://wiki.libsdl.org/SDL_DestroySemaphore, it's ...
  // unsafe to destroy semaphore if there's currently a thread is waiting on it
  SDL_DestroySemaphore(sem_datalock);

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

