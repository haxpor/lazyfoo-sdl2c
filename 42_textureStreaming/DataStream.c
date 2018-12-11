#include "DataStream.h"
#include <stdlib.h>
#include <stdbool.h>
#include "SDL_image.h"
#include "common.h"

// speed of animation
// computed from 1/14.0
#define DELAY_ANIM .07142857142857142857

DataStream* DataStream_new()
{
  DataStream* out = malloc(sizeof(DataStream));
  
  out->current_frame = 0;
  out->anim_counting_time = 0;

  // be careful of not using this commented code as data stream's surface format needs
  // to be the same as its destination texture to copy pixel data to
  // window's pixel format might be the one you expect
  //Uint32 pixel_format = SDL_GetWindowPixelFormat(gWindow->window);

  char path[32];

  for (int i=0; i<4; i++)
  {
    memset(path, 0, 32);
    // 32-1 because leave a room for null-terminated character
    snprintf(path, 32-1, "foo_walk_%d.png", i);
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
      SDL_Log("load surface for %s failed [%s]", path, SDL_GetError());

    // continue converting format to the same as in window
    SDL_Surface* formatted_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    if (formatted_surface == NULL)
      SDL_Log("convert surface for %s failed [%s]", path, SDL_GetError());

    // free old one
    SDL_FreeSurface(surface);
    // set to our loaded images
    out->images[i] = formatted_surface;
  }

  out->current_image = out->images[0];
  return out;
}

void DataStream_update(DataStream* d, float delta_time)
{
  d->anim_counting_time += delta_time;

  if (d->anim_counting_time >= DELAY_ANIM)
  {
    // reset time
    d->anim_counting_time -= DELAY_ANIM;

    d->current_frame++;
    if (d->current_frame >= 4)
    {
      // cycle back to first frame
      d->current_frame = 0;
    }

    // set current frame
    d->current_image = d->images[d->current_frame];
  }
}

void DataStream_free(DataStream* d)
{
  // free internals
  for (int i=0; i<4; i++)
  {
    if (d->images[i] != NULL)
      SDL_FreeSurface(d->images[i]);
  }
  // this is just a pointer that pointer to (now) invalid data as we freed it
  d->current_image = NULL;
  
  // free allocated memory
  free(d);
  d = NULL;
}
