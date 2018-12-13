#include "LTexture.h"
#include "SDL.h"
#include "common.h"
#include <stdlib.h>

// variables defined in common.h
extern LWindow* gWindow;

/// underlying low level function to alloc and init things
static LTexture* LTexture_LoadFromFileARGS(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue, SDL_TextureAccess texture_access, Uint32 texture_format);
static LTexture* newblank(int width, int height, Uint32 texture_format, SDL_TextureAccess texture_access);

LTexture* LTexture_NewBlank(int width, int height, Uint32 texture_format)
{
  // create a new blank, it needs to be modificable later so only streaming type makes sense
  return newblank(width, height, texture_format, SDL_TEXTUREACCESS_STREAMING);
}

LTexture* LTexture_NewBlankRenderTarget(int width, int height, Uint32 texture_format)
{
  return newblank(width, height, texture_format, SDL_TEXTUREACCESS_TARGET);
}

LTexture* newblank(int width, int height, Uint32 texture_format, SDL_TextureAccess texture_access)
{
  // create according to input parameters
  SDL_Texture* blank_texture = SDL_CreateTexture(gWindow->renderer, texture_format, texture_access, width, height);
  if (blank_texture == NULL)
  {
    SDL_Log("Failed to create texture: %s", SDL_GetError());
    return NULL;
  }

  // allocate memory space and initialize
  LTexture* out = malloc(sizeof(LTexture));
  out->width = width;
  out->height = height;
  out->texture = blank_texture;
  out->pitch = 0;
  out->pixels = NULL;
  return out;
}

LTexture* LTexture_LoadFromFile(const char* path)
{
  return LTexture_LoadFromFileARGS(path, false, 0x00, 0xFF, 0xFF, SDL_TEXTUREACCESS_STATIC, 0);
}

LTexture* LTexture_LoadFromFileWithColorKey(const char* path, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue)
{
  return LTexture_LoadFromFileARGS(path, true, colorKeyRed, colorKeyGreen, colorKeyBlue, SDL_TEXTUREACCESS_STATIC, 0);
}

LTexture* LTexture_LoadFromFileWithColorKeyEx(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue, SDL_TextureAccess texture_access, Uint32 texture_format)
{
  return LTexture_LoadFromFileARGS(path, withColorKey, colorKeyRed, colorKeyGreen, colorKeyBlue, texture_access, texture_format);
}

LTexture* LTexture_LoadFromFileARGS(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue, SDL_TextureAccess texture_access, Uint32 texture_format)
{
  // load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path);
  if (loadedSurface == NULL)
  {
    SDL_Log("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    return NULL;
  }

  // do color key now if it's static textureaccess
  if (withColorKey && texture_access == SDL_TEXTUREACCESS_STATIC)
  {
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, colorKeyRed, colorKeyGreen, colorKeyBlue));
  }

  // result texture to get
  SDL_Texture* newTexture = NULL;
  if (texture_access == SDL_TEXTUREACCESS_STREAMING)
  {
    // if use by default to get pixel format from global main window, then get from it
    Uint32 pixel_format;
    if (texture_format == 0)
    {
      pixel_format = SDL_GetWindowPixelFormat(gWindow->window);
    }
    else
    {
      pixel_format = texture_format;
    }
    
    // convert loaded image's pixel format
    SDL_Surface* formatted_surface = SDL_ConvertSurfaceFormat(loadedSurface, pixel_format, 0);
    if (formatted_surface == NULL)
    {
      SDL_Log("Unable to convert loaded surface to display format! SDL Error: %s", SDL_GetError());
      // free surface
      SDL_FreeSurface(loadedSurface);
      return NULL;
    }

    // create blank streamable texture
    newTexture = SDL_CreateTexture(gWindow->renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, formatted_surface->w, formatted_surface->h);
    if (newTexture == NULL)
    {
      SDL_Log("Unable to create texture from %s! SDL Error: %s", path, SDL_GetError());
      // free surfaces
      SDL_FreeSurface(formatted_surface);
      SDL_FreeSurface(loadedSurface);

      return NULL;
    }

    // pixel data to be filled for holding after locking
    void* pixels_data = NULL;
    // pitch to be filld
    int pitch = 0;

    // lock texture for manipulation
    SDL_LockTexture(newTexture, &formatted_surface->clip_rect, &pixels_data, &pitch);
    // copy pixel data from surface to texture
    memcpy(pixels_data, formatted_surface->pixels, formatted_surface->pitch * formatted_surface->h);

    // do color key if necessary
    // note: we do manual color keying here because SDL_SetColorKey() will be taken into effect when blitting with SDL's functions.
    if (withColorKey)
    {
      // enable blending on texture
      // important to enable this to make this works for manual color keying
      SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

      // TODO: Add checking that pixel format should be in RGBA 32 only to make it works
      SDL_PixelFormat* mapping_format = SDL_AllocFormat(pixel_format);
      Uint32* pixels = (Uint32*)pixels_data;
      // read pixels data from its associated surface
      // see Remarks from https://wiki.libsdl.org/SDL_LockTexture
      // stating that locking texture is for write-only operation
      // thus we read from SDL_Surface instead (which is via RAM)
      const Uint32* read_pixels = (Uint32*)formatted_surface->pixels;

      int pixel_count = (pitch / 4) * formatted_surface->h;
      // map color
      Uint32 color_key = SDL_MapRGB(mapping_format, colorKeyRed, colorKeyGreen, colorKeyBlue);
      // color to swap (make it transparent)
      Uint32 transparent_color = SDL_MapRGBA(mapping_format, colorKeyRed, colorKeyGreen, colorKeyBlue, 0);
      for (int i=0; i<pixel_count; i++)
      {
        if (read_pixels[i] == color_key)
        {
          pixels[i] = transparent_color;
        }
      }

      // free mapping format
      SDL_FreeFormat(mapping_format);
      mapping_format = NULL;
    }

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

Uint32 LTexture_GetPixel32(LTexture* ltexture, unsigned int x, unsigned int y)
{
  // convert pixels to 32 bit
  Uint32 *pixels = ltexture->pixels;

  // get pixel at specified position
  return pixels[ y * (ltexture->pitch / 4) + x];
}

void LTexture_CopyPixels(LTexture* ltexture, void* pixels)
{
  // texture is locked
  if (ltexture->pixels != NULL)
  {
    // copy pixel data to texture
    memcpy(ltexture->pixels, pixels, ltexture->pitch * ltexture->height);
  }
}

void LTexture_SetAsRenderTarget(LTexture* ltexture)
{
  if (SDL_SetRenderTarget(gWindow->renderer, ltexture->texture) != 0)
  {
    // provide error message
    SDL_Log("failed to set texture as render target: %s", SDL_GetError());
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
