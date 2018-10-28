#include "LTexture.h"
#include <SDL2/SDL.h>
#include "common.h"
#include <stdlib.h>

// variables defined in common.h
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

bool LTexture_LoadFromFile(const char* path, LTexture* out)
{
  // free existing resource
  LTexture_Free(out);

  // load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    return false;
  }

  // color key image
  SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

  // create texture from surface
  SDL_Texture* newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
  if (newTexture == NULL)
  {
    printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
    // free surface
    SDL_FreeSurface(loadedSurface);
    return false;
  }

  // get image dimension
  out->width = loadedSurface->w;
  out->height = loadedSurface->h;
  // set texture to ltexture
  out->texture = newTexture;

  // free surface, we don't need it anymore
  SDL_FreeSurface(loadedSurface);

  return true;
}

bool LTexture_LoadFromRenderedText(const char* textureText, SDL_Color textColor, Uint32 wrapLength, LTexture* out)
{
  // free existing resource
  LTexture_Free(out);

  // render text surface
  SDL_Surface *textSurface = NULL;

  // if wrapLength is 0, then render without auto-wrapping support
  if (wrapLength == 0)
  {
    textSurface = TTF_RenderText_Solid(gFont, textureText, textColor);
  }
  else
  {
    textSurface = TTF_RenderText_Blended_Wrapped(gFont, textureText, textColor, wrapLength);
  }

  if (textSurface == NULL)
  {
    printf("Unable to render text surface! SDL_ttf error: %s\n", TTF_GetError());
    return false;
  }

  // create texture from surface pixels
  SDL_Texture* newTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
  if (newTexture == NULL)
  {
    printf("Unable to create texture from rendered text! SDL error: %s\n", SDL_GetError());
    SDL_FreeSurface(textSurface);
    return false;
  }

  // get image's dimensions
  out->width = textSurface->w;
  out->height = textSurface->h;
  out->texture = newTexture;

  // free surface, we don't need it anymore
  SDL_FreeSurface(textSurface);

  return true;
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

void LTexture_RenderEx(LTexture* ltexture, int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
  SDL_Rect renderQuad = { x, y, ltexture->width, ltexture->height };
  SDL_RenderCopyEx(gRenderer, ltexture->texture, NULL, &renderQuad, angle, center, flip);
}

void LTexture_ClippedRender(LTexture* ltexture, int x, int y, SDL_Rect* clip)
{
  if (ltexture == NULL || clip == NULL)
    return;

  // set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, clip->w, clip->h };
  // render to screen
  SDL_RenderCopy(gRenderer, ltexture->texture, clip, &renderQuad);
}

void LTexture_ClippedRenderEx(LTexture *ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
  SDL_Rect renderQuad = { x, y, clip->w, clip->h };
  SDL_RenderCopyEx(gRenderer, ltexture->texture, clip, &renderQuad, angle, center, flip);
}

void LTexture_SetColor(LTexture* ltexture, Uint8 red, Uint8 green, Uint8 blue)
{
  // set color for modulation
  SDL_SetTextureColorMod(ltexture->texture, red, green, blue);
}

void LTexture_SetBlendMode(LTexture* ltexture, SDL_BlendMode blending)
{
  // set blending mode
  SDL_SetTextureBlendMode(ltexture->texture, blending);
}

void LTexture_SetAlpha(LTexture* ltexture, Uint8 alpha)
{
  // modulate texture alpha
  SDL_SetTextureAlphaMod(ltexture->texture, alpha);
}

void LTexture_Free(LTexture* ltexture)
{
  // if exist then look inside their struct to deallocate, and reset stuff
  if (ltexture != NULL)
  {
    // destroy texture as attached to its texture
    if (ltexture->texture != NULL)
    {
      SDL_DestroyTexture(ltexture->texture);
      ltexture->texture = NULL;
    }

    ltexture->width = 0;
    ltexture->height = 0;
  }
}
