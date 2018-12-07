/**
 * 33 - File reading and writing
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
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

TTF_Font* custom_font;

SDL_Color normal_text_color = { 0, 0, 0, 0xff };
SDL_Color active_text_color = { 0xff, 0, 0, 0xff };

#define TOTAL_DATA 10
Sint32 data[TOTAL_DATA];

LTexture* data_textures[TOTAL_DATA];
int active_data_index = 0;

LTexture* hint_texture = NULL;
LTexture* hint2_texture = NULL;

// maximum characters for longest char possibilitndley
char temp_num_chrs[11];

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = SDL_CreateWindow("33 - File Reading and Writing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

  // custom font rather than normal gFont
  custom_font = TTF_OpenFont("Minecraft.ttf", 25);
  if (custom_font == NULL)
  {
    SDL_Log("Failed to load VCR_MONO.ttf font: %s", TTF_GetError());
    return false;
  }

  // open file for reading in binary
  SDL_RWops* file = SDL_RWFromFile("nums.bin", "r+b");
  if (file == NULL)
  {
    SDL_Log("no file exists yet, create a new now");

    // create file for writing
    file = SDL_RWFromFile("nums.bin", "w+b");
    if (file != NULL)
    {
      SDL_Log("created new file for writing");

      // clear values in data to zero first
      memset(data, 0, sizeof(data));
      // write at once
      SDL_RWwrite(file, data, sizeof(Sint32), TOTAL_DATA);

      // close file handler
      SDL_RWclose(file);
    }
    else
    {
      SDL_Log("unable to create file for writing: %s", SDL_GetError());
      return false;
    }
  }
  // file exists
  else
  {
    // load data
    SDL_Log("file exists, load data from file");

    // read at once
    SDL_RWread(file, data, sizeof(Sint32), TOTAL_DATA);

    // close file handler
    SDL_RWclose(file);
  }

  // hint texture
  hint_texture = LTexture_LoadFromRenderedText_withFont("Enter data:", custom_font, normal_text_color, SCREEN_WIDTH-10);
  if (hint_texture == NULL)
  {
    SDL_Log("Failed to generate hint texture: %s", SDL_GetError());
    return false;
  }
  // hint2 texture
  SDL_Color hint_text_color = {128, 128, 128, 0xff};
  hint2_texture = LTexture_LoadFromRenderedText_withFont("Use Up/Down to switch between data. Use Left/Right to decrement/increment value.", custom_font, hint_text_color, SCREEN_WIDTH*0.7);
  if (hint2_texture == NULL)
  {
    SDL_Log("Failed to generate hint2 texture: %s", SDL_GetError());
    return false;
  }

  // by now, we've loaded all data to show on screen
  for (int i=0; i<TOTAL_DATA; i++)
  {
    // free previous one first if any
    if (data_textures[i] != NULL)
    {
      LTexture_Free(data_textures[i]);
    }
    
    // use snprintf instead as itoa is not standard function
    snprintf(temp_num_chrs, 11, "%d", data[i]);

    if (active_data_index == i)
    {
      data_textures[i] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, active_text_color, SCREEN_WIDTH-10);
    }
    else
    {
      data_textures[i] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, normal_text_color, SCREEN_WIDTH-10);
    }
  }

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
      // decrement the value of individual data
      case SDLK_LEFT:
        data[active_data_index]--;

        // free existing texture first
        LTexture_Free(data_textures[active_data_index]);
        // convert new value into chars
        snprintf(temp_num_chrs, 11, "%d", data[active_data_index]);
        // re-render texture to represent new value
        data_textures[active_data_index] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, active_text_color, SCREEN_WIDTH-10);
        break;
      // increment the value of individual data
      case SDLK_RIGHT:
        data[active_data_index]++;

        // free existing texture first
        LTexture_Free(data_textures[active_data_index]);
        // convert new value into chars
        snprintf(temp_num_chrs, 11, "%d", data[active_data_index]);
        // re-render texture to represent new value
        data_textures[active_data_index] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, active_text_color, SCREEN_WIDTH-10);
        break;
      // increment active index
      case SDLK_DOWN:
        // free existing active texture
        LTexture_Free(data_textures[active_data_index]);
        // get value into chars
        snprintf(temp_num_chrs, 11, "%d", data[active_data_index]);
        // generate a new one as normal, active index passes through it now
        data_textures[active_data_index] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, normal_text_color, SCREEN_WIDTH-10);

        // change the active index
        active_data_index = (active_data_index + 1) % TOTAL_DATA;

        // free existing to-be-next active texture
        LTexture_Free(data_textures[active_data_index]);
        // get value into chars
        snprintf(temp_num_chrs, 11, "%d", data[active_data_index]);
        // generate a new one as active, active index is here now
        data_textures[active_data_index] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, active_text_color, SCREEN_WIDTH-10);
        break;
      // decrement active index
      case SDLK_UP:
        // free existing active texture
        LTexture_Free(data_textures[active_data_index]);
        // get value into chars
        snprintf(temp_num_chrs, 11, "%d", data[active_data_index]);
        // generate a new one as normal, active index passes through it now
        data_textures[active_data_index] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, normal_text_color, SCREEN_WIDTH-10);

        // change the active index
        if (active_data_index == 0)
        {
          active_data_index = TOTAL_DATA-1;
        }
        else
        {
          active_data_index--;
        }

        // free existing to-be-next active texture
        LTexture_Free(data_textures[active_data_index]);
        // get value into chars
        snprintf(temp_num_chrs, 11, "%d", data[active_data_index]);
        // generate a new one as active, active index is here now
        data_textures[active_data_index] = LTexture_LoadFromRenderedText_withFont(temp_num_chrs, custom_font, active_text_color, SCREEN_WIDTH-10);
        break;
      case SDLK_ESCAPE:
        quit = true;
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
  }
#endif

  LTexture_Render(hint_texture, SCREEN_WIDTH/2-hint_texture->width/2, 10);
  for (int i=0; i<TOTAL_DATA; i++)
  {
    if (data_textures[i] != NULL)
    {
      LTexture_Render(data_textures[i], SCREEN_WIDTH/2-data_textures[i]->width/2, 10 + hint_texture->height + 10 + i*hint_texture->height);
    }
  }
  LTexture_Render(hint2_texture, SCREEN_WIDTH/2-hint2_texture->width/2, SCREEN_HEIGHT-10-hint2_texture->height);
}

void close()
{
  // save data to file first before quit
  SDL_RWops* file = SDL_RWFromFile("nums.bin", "w+b");
  if (file != NULL)
  {
    SDL_Log("write to file before quitting");

    // save data
    SDL_RWwrite(file, data, sizeof(Sint32), TOTAL_DATA);

    // close file
    SDL_RWclose(file);
  }
  else
  {
    SDL_Log("failed to open file to write before quitting.");
  }

  // free font
  if (gFont != NULL)
  {
    TTF_CloseFont(gFont);
    gFont = NULL;
  }
  // free our custom font
  if (custom_font != NULL)
  {
    TTF_CloseFont(custom_font);
    custom_font = NULL;
  }

  if (hint_texture != NULL)
    LTexture_Free(hint_texture);
  if (hint2_texture != NULL)
    LTexture_Free(hint2_texture);
  for (int i=0; i<TOTAL_DATA; i++)
  {
    if (data_textures[i] != NULL)
    {
      LTexture_Free(data_textures[i]);
    }
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

