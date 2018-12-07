/**
 * 34 - Audio Recording
 */

#include "SDL.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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

// max number of supported recording devices
const int MAX_RECORDING_DEVICES = 10;

// max recording time
const int MAX_RECORDING_SECONDS = 5;

// max recording time plus padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;

// various state of recording actions
enum RecordingState
{
  SELECTING_DEVICE,
  STOPPED,
  RECORDING,
  RECORDED,
  PLAYBACK,
  ERROR
};

SDL_Color text_color = {0,0,0,0xff};

LTexture* prompt_texture = NULL;
// texture to hold device name for recording devices
LTexture* recording_device_name_textures[MAX_RECORDING_DEVICES];
int available_recording_device_count = 0;

// received audio spec
SDL_AudioSpec received_recording_spec;
SDL_AudioSpec received_playback_spec;
// recording/playback callbacks
void audio_recording_callback(void* userdata, Uint8* stream, int len);
void audio_playback_callback(void* userdata, Uint8* stream, int len);

// recording data buffer
Uint8* recording_buffer = NULL;
// size of data buffer
Uint32 buffer_byte_size = 0;
// position in data buffer
Uint32 buffer_byte_position = 0;
// maximum position in data buffer for recording
Uint32 buffer_byte_max_position = 0;

// recording state
enum RecordingState recording_state = SELECTING_DEVICE;
// audio device IDs
SDL_AudioDeviceID recording_deviceid = 0;
SDL_AudioDeviceID playback_deviceid = 0;

void audio_recording_callback(void* userdata, Uint8* stream, int len)
{
  // copy audio from stream
  memcpy(&recording_buffer[buffer_byte_position], stream, len);

  // move along buffer
  buffer_byte_position += len;
}

void audio_playback_callback(void* userdata, Uint8* stream, int len)
{
  // copy audio to stream
  memcpy(stream, &recording_buffer[buffer_byte_position], len);

  // move along buffer
  buffer_byte_position += len;
}

bool init() {
  // initialize sdl
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    return false;
  }

  // create window
  gWindow = SDL_CreateWindow("34 - Audio Recording", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
  gFont = TTF_OpenFont("../Minecraft.ttf", 16);
  if (gFont == NULL)
  {
    SDL_Log("Failed to load 8bitwonder.ttf font: %s", TTF_GetError());
    return false;
  }

  // generate starting prompt texture
  prompt_texture = LTexture_LoadFromRenderedText("Select your recording device:", text_color, SCREEN_WIDTH-10);
  if (prompt_texture == NULL)
  {
    SDL_Log("Failed to generate prompt texture");
    return false;
  }

  // get capture device count
  // note: pass positive number to get recording device, pass negative or zero to get playback device
  available_recording_device_count = SDL_GetNumAudioDevices(SDL_TRUE);
  if (available_recording_device_count < 1)
  {
    SDL_Log("Unable to get audio capture device! %s", SDL_GetError());
    // return now as we can't proceed with this sample app
    return false;
  }
  else
  {
    // cap recording device count
    if (available_recording_device_count > MAX_RECORDING_DEVICES)
    {
      available_recording_device_count = MAX_RECORDING_DEVICES;
    }

    // generate texture for recording devices' name
    char temp_chrs[120];
    for (int i=0; i<available_recording_device_count; i++)
    {
      // get name of device
      // specify SDL_TRUE to get name of recording device
      const char* device_name = SDL_GetAudioDeviceName(i, SDL_TRUE);
      // form string to show on screen
      snprintf(temp_chrs, 100, "[%d] : %s", i, device_name);

      if (device_name != NULL)
      {
        // note: no need to free this string, it's managed internally. Check its official doc on sdl2 doc site.
        recording_device_name_textures[i] = LTexture_LoadFromRenderedText(temp_chrs, text_color, SCREEN_WIDTH-10);
      }
    }
  }

  return true;
}

void update(float deltaTime)
{
  // checking to finish the recording process after it has been set to record
  if (recording_state == RECORDING)
  {
    // if recorded passed 5 seconds mark
    if (buffer_byte_position > buffer_byte_max_position)
    {
      // lock callback (different thread)
      SDL_LockAudioDevice(recording_deviceid);

      // stop recording audio
      SDL_PauseAudioDevice(recording_deviceid, SDL_TRUE);

      // set state
      LTexture_Free(prompt_texture);
      prompt_texture = LTexture_LoadFromRenderedText("Press 1 to play back. Press 2 to record again.", text_color, SCREEN_WIDTH-10);
      recording_state = RECORDED;

      // unlock callback
      SDL_UnlockAudioDevice(recording_deviceid);
    }
  }
  else if (recording_state == PLAYBACK)
  {
    // check if finish playback
    if (buffer_byte_position > buffer_byte_max_position)
    {
      // lock callback (different thread)
      SDL_LockAudioDevice(playback_deviceid);

      // stop playback
      SDL_PauseAudioDevice(playback_deviceid, SDL_TRUE);

      // set state
      LTexture_Free(prompt_texture);
      prompt_texture = LTexture_LoadFromRenderedText("Press 1 to play back. Press 2 to record again.", text_color, SCREEN_WIDTH-10);
      recording_state = RECORDED;

      // unlock callback
      SDL_UnlockAudioDevice(playback_deviceid);
    }
  }
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

  if (!quit)
  {
    switch (recording_state)
    {
      case SELECTING_DEVICE:
        // on key press
        if (e->type == SDL_KEYDOWN)
        {
          // handle key press from 0 to 9
          if (e->key.keysym.sym >= SDLK_0 && e->key.keysym.sym <= SDLK_9)
          {
            // get selection index
            int index = e->key.keysym.sym - SDLK_0;
            // index is valid
            if (index < available_recording_device_count)
            {
              // default audio spec
              SDL_AudioSpec desired_recording_spec;
              SDL_zero(desired_recording_spec);
              desired_recording_spec.freq = 44100;
              desired_recording_spec.format = AUDIO_F32;
              desired_recording_spec.channels = 2;
              desired_recording_spec.samples = 4096;
              desired_recording_spec.callback = audio_recording_callback;

              // open recording device
              recording_deviceid = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(index, SDL_TRUE), SDL_TRUE, &desired_recording_spec, &received_recording_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
              // if error
              if (recording_deviceid == 0)
              {
                SDL_Log("failed to open audio device [%d]: %s", index, SDL_GetError());

                // re-generate prompt texture to reflect the current status of operation
                LTexture_Free(prompt_texture);
                prompt_texture = LTexture_LoadFromRenderedText("Failed to open recording device!", text_color, SCREEN_WIDTH-10);
                recording_state = ERROR;
              }
              // device opens successfully
              else
              {
                // default audio spec
                SDL_AudioSpec desired_playback_spec;
                SDL_zero(desired_playback_spec);
                desired_playback_spec.freq = 44100;
                desired_playback_spec.format = AUDIO_F32;
                desired_playback_spec.channels = 2;
                desired_playback_spec.samples = 4096;
                desired_playback_spec.callback = audio_playback_callback;

                // open playback device
                // note: pass NULL to indicate that we don't care which playback device we'd got, just get the first available one
                playback_deviceid = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desired_playback_spec, &received_playback_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
                // error
                if (playback_deviceid == 0)
                {
                  SDL_Log("failed to open audio playback device %s", SDL_GetError());

                  // re-generate prompt texture to reflect the current status of operation
                  LTexture_Free(prompt_texture);
                  prompt_texture = LTexture_LoadFromRenderedText("Failed to open playback device!", text_color, SCREEN_WIDTH-10);
                  recording_state = ERROR;
                }
                // device playback opens successfully
                else
                {
                  // calculate per sample bytes
                  int bytes_per_sample = received_recording_spec.channels * (SDL_AUDIO_BITSIZE(received_recording_spec.format) / 8);

                  // calculate bytes per second
                  int bytes_per_second = received_recording_spec.freq * bytes_per_sample;

                  // calculate buffer size
                  buffer_byte_size = RECORDING_BUFFER_SECONDS * bytes_per_second;

                  // calculate max buffer use
                  buffer_byte_max_position = MAX_RECORDING_SECONDS * bytes_per_second;

                  // allocate and initialize byte buffer
                  recording_buffer = malloc(buffer_byte_size);
                  memset(recording_buffer, 0, buffer_byte_size);

                  // update status via prompt texture
                  LTexture_Free(prompt_texture);
                  prompt_texture = LTexture_LoadFromRenderedText("Press 1 to record for 5 seconds.", text_color, SCREEN_WIDTH-10);
                  recording_state = STOPPED;
                }
              }
            }
          }
        }
        break;
      case STOPPED:
        if (e->type == SDL_KEYDOWN)
        {
          // start recording
          if (e->key.keysym.sym == SDLK_1)
          {
            // go back to the beginning of buffer
            buffer_byte_position = 0;

            // start recording
            SDL_PauseAudioDevice(recording_deviceid, SDL_FALSE);
            
            // set state
            LTexture_Free(prompt_texture);
            prompt_texture = LTexture_LoadFromRenderedText("Recording...", text_color, SCREEN_WIDTH-10);
            recording_state = RECORDING;
          }
        }
        break;
      case RECORDED:
        if (e->type == SDL_KEYDOWN)
        {
          // press 1 to play back
          if (e->key.keysym.sym == SDLK_1)
          {
            // go back to beginning of buffer
            buffer_byte_position = 0;

            // start playback
            SDL_PauseAudioDevice(playback_deviceid, SDL_FALSE);

            // set state
            LTexture_Free(prompt_texture);
            prompt_texture = LTexture_LoadFromRenderedText("Playing...", text_color, SCREEN_WIDTH-10);
            recording_state = PLAYBACK;
          }
          // press 2 to re-record again
          else if (e->key.keysym.sym == SDLK_2)
          {
            // reset buffer
            buffer_byte_position = 0;
            memset(recording_buffer, 0, buffer_byte_size);

            // start recording
            SDL_PauseAudioDevice(recording_deviceid, SDL_FALSE);

            // set state
            LTexture_Free(prompt_texture);
            prompt_texture = LTexture_LoadFromRenderedText("Recording...", text_color, SCREEN_WIDTH-10);
            recording_state = RECORDING;
          }
        }
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
    LTexture_Render(fpsTexture, SCREEN_WIDTH - fpsTexture->width - 5, 10);
    LTexture_Free(fpsTexture);
  }
#endif

  LTexture_Render(prompt_texture, SCREEN_WIDTH/2-prompt_texture->width/2, 10);

  for (int i=0; i<available_recording_device_count; i++)
  {
    LTexture_Render(recording_device_name_textures[i], 10, 10 + prompt_texture->height + 10);
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

  if (prompt_texture != NULL)
    LTexture_Free(prompt_texture);

  for (int i=0; i<available_recording_device_count; i++)
  {
    if (recording_device_name_textures[i] != NULL)
      LTexture_Free(recording_device_name_textures[i]);
  }

  // free audio buffer
  if (recording_buffer != NULL)
  {
    free(recording_buffer);
    recording_buffer = NULL;
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

