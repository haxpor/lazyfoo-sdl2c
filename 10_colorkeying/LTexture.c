#include "LTexture.h"
#include <SDL2/SDL.h>
#include "common.h"
#include <stdlib.h>

// variables defined in common.h
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

LTexture* LTexture_LoadFromFile(const char* path)
{
  // we have no need to free existing texture loaded
  // as it's not OOP, we don't hold anything for anyone

  // load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    return NULL;
  }

  // color key image
  SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

  // result struct to return
  LTexture* ltexture = NULL;

  // create texture from surface
  SDL_Texture* newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
  if (newTexture == NULL)
  {
    printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
  }
  else
  {
    // create a new struct on heap
    // user needs to call free() on it when finishes using
    ltexture = malloc(sizeof(LTexture));

    // get image dimension
    ltexture->width = loadedSurface->w;
    ltexture->height = loadedSurface->h;
    // set texture to ltexture
    ltexture->texture = newTexture;
  }

  // free surface
  SDL_FreeSurface(loadedSurface);

  return ltexture;
}

void LTexture_Render(LTexture* texture, int x, int y)
{
  if (texture == NULL)
  {
    return;
  }

  // set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, texture->width, texture->height };	
  SDL_RenderCopy(gRenderer, texture->texture, NULL, &renderQuad);
}

void LTexture_Free(LTexture* texture)
{
  // if texture exists then deallocate it first
  if (texture != NULL)
  {
    SDL_DestroyTexture(texture->texture);
    texture->texture = NULL;

    // free it
    free(texture);
    // set null
    texture = NULL;
  }
}
