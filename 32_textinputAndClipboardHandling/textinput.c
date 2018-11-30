/**
 * 32 - Text Input and Clipboard Handling
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include "common.h"
#include "LTexture.h"
#include "LTimer.h"
#include <string.h>

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

// limit 20 characters to be entered (1 for null-terminated character)
#define TEXT_BUFFER 21
char text_input[TEXT_BUFFER];
SDL_Color text_color = { 0, 0, 0, 0xff };
LTexture* hinttext_texture = NULL;
LTexture* text_texture = NULL;
bool render_text = false;

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

  // hint text
  hinttext_texture = LTexture_LoadFromRenderedText("Enter text", text_color, 0);
  if (hinttext_texture == NULL)
  {
    SDL_Log("Failed to load hint text");
    return false;
  }

  // set the current string to our holding string
  strncpy(text_input, "Some text here", 14);
  // initially render text into texture
  text_texture = LTexture_LoadFromRenderedText(text_input, text_color, 0);

  // enable text input
  SDL_StartTextInput();

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
      // if there's some characters already there, then remove it
      case SDLK_BACKSPACE:
      {
        size_t str_length = strlen(text_input);
        if (str_length > 0)
        {
          text_input[str_length-1] = '\0';
          render_text = true;
        }
        break;
      }
      // copy text already entered into clipboard
      case SDLK_c:
        if (SDL_GetModState() & KMOD_CTRL)
        {
          SDL_SetClipboardText(text_input);
        }
        break;
      // handle paste
      case SDLK_v:
        if (SDL_GetModState() & KMOD_CTRL)
        {
          char* text_onclipboard = SDL_GetClipboardText();
          if (text_onclipboard != NULL)
          {
            // copy at most TEXT_BUFFER
            strncpy(text_input, text_onclipboard, TEXT_BUFFER);
            render_text = true;
          }
        }
        break;
      case SDLK_ESCAPE:
        quit = true;
        break;
    }
  }
  else if (e->type == SDL_TEXTINPUT)
  {
    // not copy or pasting
    char c_chk = e->text.text[0];
    // limit number of characters to be appended and ignore copy and paste event as we handled it previously
    if (strlen(text_input) < TEXT_BUFFER-1 && !((c_chk == 'c' ||
        c_chk == 'C' ||
        c_chk == 'v' ||
        c_chk == 'V') && (SDL_GetModState() & KMOD_CTRL)))
    {
      // append character
      strncat(text_input, &c_chk, 1);
      render_text = true;
    }
  }

  // if render_text flag is set then we generate texture to render it
  if (render_text)
  {
    if (strlen(text_input) > 0)
    {
      // free the previous rendered texture first
      if (text_texture != NULL)
      {
        LTexture_Free(text_texture);
      }
      // render a new texture from text
      text_texture = LTexture_LoadFromRenderedText(text_input, text_color, SCREEN_WIDTH-10);
    }
    // text is empty, then render a space as workaround due to SDL_ttf won't render empty string
    else
    {
      // free previous rendered texture first (if any)
      if (text_texture != NULL)
      {
        LTexture_Free(text_texture);
      }

      // render a new texture from text
      text_texture = LTexture_LoadFromRenderedText(" ", text_color, 0);
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
  }
#endif

  LTexture_Render(hinttext_texture, SCREEN_WIDTH/2-hinttext_texture->width/2, 10);
  LTexture_Render(text_texture, SCREEN_WIDTH/2-text_texture->width/2, 10 + hinttext_texture->height + 10);
}

void close()
{
  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }

  if (hinttext_texture != NULL)
  {
    LTexture_Free(hinttext_texture);
  }
  if (text_texture != NULL)
  {
    LTexture_Free(text_texture);
  }

  // disable text input
  SDL_StopTextInput();

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

