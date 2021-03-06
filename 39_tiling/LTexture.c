#include "LTexture.h"
#include "SDL.h"
#include "common.h"
#include <stdlib.h>

// variables defined in common.h
extern LWindow* gWindow;

static LTexture* LTexture_LoadFromFileColorKeyFlag(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue);

LTexture* LTexture_LoadFromFile(const char* path)
{
  return LTexture_LoadFromFileColorKeyFlag(path, false, 0x00, 0xFF, 0xFF);
}

LTexture* LTexture_LoadFromFileWithColorKey(const char* path, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue)
{
  return LTexture_LoadFromFileColorKeyFlag(path, true, colorKeyRed, colorKeyGreen, colorKeyBlue);
}

LTexture* LTexture_LoadFromFileColorKeyFlag(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue)
{
  // load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL)
  {
    SDL_Log("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    return NULL;
  }

  if (withColorKey)
  {
    // color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, colorKeyRed, colorKeyGreen, colorKeyBlue));
  }

  // create texture from surface
  SDL_Texture* newTexture = SDL_CreateTextureFromSurface(gWindow->renderer, loadedSurface);
  if (newTexture == NULL)
  {
    SDL_Log("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
    // free surface
    SDL_FreeSurface(loadedSurface);
    return NULL;
  }

  // allocate heap for LTexture
  LTexture* out = malloc(sizeof(LTexture));
  // get image dimension
  out->width = loadedSurface->w;
  out->height = loadedSurface->h;
  // set texture to ltexture
  out->texture = newTexture;

  // free surface, we don't need it anymore
  SDL_FreeSurface(loadedSurface);

  return out;
}

#ifndef DISABLE_SDL_TTF_LIB
LTexture* LTexture_LoadFromRenderedText(const char* textureText, SDL_Color textColor, Uint32 wrapLength)
{
  return LTexture_LoadFromRenderedText_withFont(textureText, gFont, textColor, wrapLength);
}

LTexture* LTexture_LoadFromRenderedText_withFont(const char* textureText, TTF_Font* font, SDL_Color textColor, Uint32 wrapLength)
{
  // render text surface
  SDL_Surface *textSurface = NULL;

  // if wrapLength is 0, then render without auto-wrapping support
  if (wrapLength == 0)
  {
    textSurface = TTF_RenderText_Solid(font, textureText, textColor);
  }
  else
  {
    textSurface = TTF_RenderText_Blended_Wrapped(font, textureText, textColor, wrapLength);
  }

  if (textSurface == NULL)
  {
    SDL_Log("Unable to render text surface! SDL_ttf error: %s\n", TTF_GetError());
    return NULL;
  }

  // create texture from surface pixels
  SDL_Texture* newTexture = SDL_CreateTextureFromSurface(gWindow->renderer, textSurface);
  if (newTexture == NULL)
  {
    SDL_Log("Unable to create texture from rendered text! SDL error: %s\n", SDL_GetError());
    SDL_FreeSurface(textSurface);
    return NULL;
  }

  // allocate LTexture on heap
  LTexture* out = malloc(sizeof(LTexture));
  // get image's dimensions
  out->width = textSurface->w;
  out->height = textSurface->h;
  out->texture = newTexture;

  // free surface, we don't need it anymore
  SDL_FreeSurface(textSurface);

  return out;
}
#endif

void LTexture_Render(LTexture* texture, int x, int y)
{
  // set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, texture->width, texture->height };	
  SDL_RenderCopy(gWindow->renderer, texture->texture, NULL, &renderQuad);
}

void LTexture_RenderEx(LTexture* ltexture, int x, int y, float scale, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
  // calculate new x and y
  int new_x = (x + ltexture->width/2) - (ltexture->width/2 * scale);
  int new_y = (y + ltexture->height/2) - (ltexture->height/2 * scale);

  SDL_Rect renderQuad = { new_x, new_y, ltexture->width * scale, ltexture->height * scale };

  SDL_RenderCopyEx(gWindow->renderer, ltexture->texture, NULL, &renderQuad, angle, center, flip);
}

void LTexture_ClippedRender(LTexture* ltexture, int x, int y, SDL_Rect* clip)
{
  // set rendering space and render to screen
  SDL_Rect renderQuad = { x, y, clip->w, clip->h };
  // render to screen
  SDL_RenderCopy(gWindow->renderer, ltexture->texture, clip, &renderQuad);
}

void LTexture_ClippedRenderEx(LTexture *ltexture, int x, int y, float scale, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
  // calculate new x and y
  int new_x = (x + clip->w/2) - (clip->w/2 * scale);
  int new_y = (y + clip->h/2) - (clip->h/2 * scale);

  SDL_Rect renderQuad = { new_x, new_y, clip->w * scale, clip->h * scale };
  SDL_RenderCopyEx(gWindow->renderer, ltexture->texture, clip, &renderQuad, angle, center, flip);
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
  // destroy texture as attached to its texture
  if (ltexture->texture != NULL)
  {
    SDL_DestroyTexture(ltexture->texture);
    ltexture->texture = NULL;
  }

  free(ltexture);
  ltexture = NULL; 
}
