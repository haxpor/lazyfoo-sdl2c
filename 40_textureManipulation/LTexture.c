#include "LTexture.h"
#include "SDL.h"
#include "common.h"
#include <stdlib.h>

// variables defined in common.h
extern LWindow* gWindow;

/// underlying low level function to alloc and init things
static LTexture* LTexture_LoadFromFileARGS(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue, SDL_TextureAccess texture_access);

LTexture* LTexture_LoadFromFile(const char* path)
{
  return LTexture_LoadFromFileARGS(path, false, 0x00, 0xFF, 0xFF, SDL_TEXTUREACCESS_STATIC);
}

LTexture* LTexture_LoadFromFileWithColorKey(const char* path, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue)
{
  return LTexture_LoadFromFileARGS(path, true, colorKeyRed, colorKeyGreen, colorKeyBlue, SDL_TEXTUREACCESS_STATIC);
}

LTexture* LTexture_LoadFromFileWithColorKeyEx(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue, SDL_TextureAccess texture_access)
{
  return LTexture_LoadFromFileARGS(path, withColorKey, colorKeyRed, colorKeyGreen, colorKeyBlue, texture_access);
}

LTexture* LTexture_LoadFromFileARGS(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue, SDL_TextureAccess texture_access)
{
  // load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL)
  {
    SDL_Log("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    return NULL;
  }

  // color key image
  if (withColorKey)
  {
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, colorKeyRed, colorKeyGreen, colorKeyBlue));
  }

  // result texture to get
  SDL_Texture* newTexture = NULL;
  if (texture_access == SDL_TEXTUREACCESS_STREAMING)
  {
    // get window pixel format
    // TODO: Modify this function to accept window as input
    Uint32 window_pixelformat = SDL_GetWindowPixelFormat(gWindow->window);

    SDL_Surface* formatted_surface = SDL_ConvertSurfaceFormat(loadedSurface, window_pixelformat, 0);
    if (formatted_surface == NULL)
    {
      SDL_Log("Unable to convert loaded surface to display format! SDL Error: %s", SDL_GetError());
      // free surface
      SDL_FreeSurface(loadedSurface);
      return NULL;
    }

    // create blank streamable texture
    newTexture = SDL_CreateTexture(gWindow->renderer, window_pixelformat, SDL_TEXTUREACCESS_STREAMING, formatted_surface->w, formatted_surface->h);
    if (newTexture == NULL)
    {
      SDL_Log("Unable to create texture from %s! SDL Error: %s", path, SDL_GetError());
      // free surface
      SDL_FreeSurface(loadedSurface);
      return NULL;
    }

    // pixel data to be filled for holding after locking
    void* pixels_data = NULL;
    // pitch to be filld
    int pitch = 0;

    // lock texture for manipulation
    SDL_LockTexture(newTexture, NULL, &pixels_data, &pitch);
    // copy pixel data from surface to texture
    memcpy(pixels_data, formatted_surface->pixels, formatted_surface->pitch * formatted_surface->h);
    // unlock texture to finish the job
    SDL_UnlockTexture(newTexture);
    // pointer to pixel data becomes invalid now
    pixels_data = NULL;
    // pitch data becomes invalid
    pitch = 0;

    // allocate memory space for LTexture
    LTexture* out = malloc(sizeof(LTexture));
    // get image dimension
    out->width = formatted_surface->w;
    out->height = formatted_surface->h;
    // set texture to ltexture
    out->texture = newTexture;
    // init attributes
    out->pixels = NULL;
    out->pitch = 0;

    // free both old surface and new formatted surface
    SDL_FreeSurface(formatted_surface);
    SDL_FreeSurface(loadedSurface);

    return out;
  }
  // otherwise create with static texture access
  // currently SDL_TEXTUREACCESS_TARGET we don't support yet, so if input value is that
  // we create with SDL_TEXTUREACCESS_STATIC one
  else
  {
    // create texture from surface
    newTexture = SDL_CreateTextureFromSurface(gWindow->renderer, loadedSurface);
    if (newTexture == NULL)
    {
      SDL_Log("Unable to create texture from %s! SDL Error: %s", path, SDL_GetError());
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
    // init attributes
    out->pixels = NULL;
    out->pitch = 0;

    // free surface, we don't need it anymore
    SDL_FreeSurface(loadedSurface);

    return out;
  }
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

bool LTexture_LockTexture(LTexture* ltexture)
{
  // if texture is already locked
  if (ltexture->pixels != NULL)
  {
    SDL_Log("Texture is already locked");
    return false;
  }
  // lock texture
  else
  {
    if (SDL_LockTexture(ltexture->texture, NULL, &ltexture->pixels, &ltexture->pitch) != 0)
    {
      SDL_Log("Unable to lock texture! %s", SDL_GetError());
      return false;
    }
  }

  return true;
}

bool LTexture_UnlockTexture(LTexture* ltexture)
{
  // check if texture is not locked yet
  if (ltexture->pixels == NULL)
  {
    SDL_Log("Texture is not locked");
    return false;
  }
  else
  {
    SDL_UnlockTexture(ltexture->texture);
    ltexture->pixels = NULL;
    ltexture->pitch = 0;

    return true;
  }
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
