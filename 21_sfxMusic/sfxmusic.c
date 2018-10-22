/**
 * 21 - Sound effects and Music
 *
 * Credits music "bensound-ukulele.mp3" from www.bensound.com
 *  See https://www.bensound.com/royalty-free-music/track/ukulele
 */

#include "SDL.h"
#include "SDL_mixer.h"
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

// -- functions
bool init();
bool setup();
void update();
void handleEvent(SDL_Event *e);
void render();
void close();

// -- variables
bool quit = false;

// refer to variables declared and defined elsewhere (in common.h)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

#ifndef DISABLE_SDL_TTF_LIB
extern TTF_Font* gFont;
#endif

// global texture for button
LTexture* gTexture = NULL;

// the music that will be played
Mix_Music *gMusic = NULL;

// the sound effects that will be played
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;

bool init() {
	// initialize sdl
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

  // initialize SDL_mixer
  // set to use good enough frequency which is 22050, instead of 44100
  // it uses MIX_DEFAULT_FORMAT which is AUDIO_S16SYS (signed 16-bit samples, in system byte order)
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
  {
    SDL_Log("SDL_mixer could not initialize! Error: %s", Mix_GetError());
    return false;
  }

  // initialize SDL_mixer to support mp3 format
  int audioFlags = MIX_INIT_MP3;
  if ((Mix_Init(audioFlags) & audioFlags) != audioFlags)
  {
    SDL_Log("Warning: some audio format won't be supported by SDL_mixer [%s]", Mix_GetError());
  }
	
	return true;
}

// include any asset loading sequence, and preparation code here
bool setup()
{
	// load buttons texture
	gTexture = LTexture_LoadFromFile("prompt.png");
	if (gTexture == NULL)
	{
		SDL_Log("Failed to load texture");
		return false;
	}

  // load music
  gMusic = Mix_LoadMUS("bensound-ukulele.mp3");
  if (gMusic == NULL)
  {
    SDL_Log("Failed to load music [bensound-ukulele.mp3]. Error: %s", Mix_GetError());
    return false;
  }

  // load sfx, scratch.wav
  gScratch = Mix_LoadWAV("scratch.wav");
  if (gScratch == NULL)
  {
    SDL_Log("Failed to load sfx [scratch.wav]. Error: %s", Mix_GetError());
    return false;
  }

  // load sfx, high.wav
  gHigh = Mix_LoadWAV("high.wav");
  if (gHigh == NULL)
  {
    SDL_Log("Failed to load sfx [high.wav]. Error %s", Mix_GetError());
    return false;
  }

  // load sfx, medium.wav
  gMedium = Mix_LoadWAV("medium.wav");
  if (gMedium == NULL)
  {
    SDL_Log("Failed to load sfx [medium.wav]. Error %s", Mix_GetError());
    return false;
  }

  // load sfx, low.wav
  gLow = Mix_LoadWAV("low.wav");
  if (gLow == NULL)
  {
    SDL_Log("Failed to laod sfx [low.wav]. Error %s", Mix_GetError());
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
	// user presses a key
	else if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
      // play high sfx
      case SDLK_1:
        // -1 means play at first unreserved mixing channel found
        // 0 means playing the sample once (1 is twice, and -1 is infinite loop)
        // check https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_28.html for more detail
        Mix_PlayChannel(-1, gHigh, 0);
        break;

      case SDLK_2:
        Mix_PlayChannel(-1, gMedium, 0);
        break;

      case SDLK_3:
        Mix_PlayChannel(-1, gLow, 0);
        break;

      case SDLK_4:
        Mix_PlayChannel(-1, gScratch, 0);
        break;

      case SDLK_8:
        if (Mix_FadeOutMusic(1000) == 1)
        {
          SDL_Log("Fade out music");
        }
        break;

      case SDLK_9:
        // if there is no music playing
        if (Mix_PlayingMusic() == 0)
        {
          // play music
          Mix_PlayMusic(gMusic, -1);
          SDL_Log("Play music");
        }
        // otherwise if music is playing
        else
        {
          // if the music is paused
          // note: Mix_PlayingMusic() won't tell anything about the music is being paused or not
          if (Mix_PausedMusic() == 1)
          {
            // resume music
            Mix_ResumeMusic();
            SDL_Log("Resume music");
          }
          // otherwise music is playing
          else
          {
            // pause music
            Mix_PauseMusic();
            SDL_Log("Pause music");
          }
        }
        break;

      case SDLK_0:
        Mix_HaltMusic();
        SDL_Log("Halt music");
        break;

		  case SDLK_ESCAPE:
			  quit = true;
        break;
    }
	}
}

void render()
{
	// clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(gRenderer);

  LTexture_Render(gTexture, 0, 0);
}

void close()
{
	// clear resource of our textures
	if (gTexture != NULL)
		LTexture_Free(gTexture);

  // free music
  if (gMusic != NULL)
  {
    Mix_FreeMusic(gMusic);
    gMusic = NULL;
  }

  // free sfxs
  if (gScratch != NULL)
  {
    Mix_FreeChunk(gScratch);
    gScratch = NULL;
  }
  if (gHigh != NULL)
  {
    Mix_FreeChunk(gHigh);
    gHigh = NULL;
  }
  if (gMedium != NULL)
  {
    Mix_FreeChunk(gMedium);
    gMedium = NULL;
  }
  if (gLow != NULL)
  {
    Mix_FreeChunk(gLow);
    gLow = NULL;
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
  Mix_Quit();
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

