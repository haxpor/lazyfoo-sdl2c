/*
 * LTexture
 *
 * It represents a texture wrapper with functionality related to it.
 */

#ifndef LTexture_h_
#define LTexture_h_

#include <stdbool.h>
#include "SDL_image.h"

#ifndef DISABLE_SDL_TTF_LIB
#include "SDL_ttf.h"
#endif

// i know guys, we will care about byte alignment of struct later ;)
typedef struct {
	SDL_Texture* texture;
  /// width of texture
	int width;
  
  /// height of texture
	int height;

  /// pitch of texture
  /// will be present and valid only when lock texture via LTexture_LockTexture()
  int pitch;

  /// opaque pixel data. It can be NULL if at the moment texture isn't locked.
  /// Used this for *write-only* operation.
  /// will be present only when lock texture via LTexture_LockTexture()
  void* pixels;
} LTexture;

/*
 * Load texture at the specified path.
 * out will be filled with newly created LTexture.
 *
 * Texture format to be created will be the same or similar to main global window.
 *
 * colorKey.... is for color key for such texture
 * Return newly created LTexture as loaded from file.
 */
extern LTexture* LTexture_LoadFromFile(const char* path);
extern LTexture* LTexture_LoadFromFileWithColorKey(const char* path, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue);

///
/// Load texture at the specified path with extra arguments.
///
/// \param path Path to image file
/// \param withColorKey True to also set color key, otherwise false thus colorKeyRed, colorKeyGreen and colorKeyBlue will be ignored.
/// \param colorKeyRed Color key red component 0-255
/// \param colorKeyGreen Color key green component 0-255
/// \param colorKeyBlue Color key blue component 0-255
/// \param texture_access Texture access
/// \param texture_format Texture format to create. Set to 0 to create texture format the same as in global main window.
/// \return Newly created LTexture.
///
extern LTexture* LTexture_LoadFromFileWithColorKeyEx(const char* path, bool withColorKey, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue, SDL_TextureAccess texture_access, Uint32 texture_format);

#ifndef DISABLE_SDL_TTF_LIB
/*
 * Load texture from rendered text, and color.
 * It will use global font by default.
 * out will be filled with newly created LTexture.
 * wrapLength is the length in pixel to do wrapping with newlines. Set to 0 to disable auto-wrapping.
 * Return newly created LTexture as loaded from rendered text.
 */
extern LTexture* LTexture_LoadFromRenderedText(const char* textureText, SDL_Color textColor, Uint32 wrapLength);

// Load texture from rendered text, color with custom font.
// out - it will be filled with newly created LTexture.
// wrapLength - the length in pixel to do wrapping with newline. Set to 0 to disable auto-wrapping.
// Return newly created LTexture as loaded from rendered text.
extern LTexture* LTexture_LoadFromRenderedText_withFont(const char* textureText, TTF_Font* font, SDL_Color textColor, Uint32 wrapLength);
#endif

/*
 * Render LTexture at given point.
 */
extern void LTexture_Render(LTexture* texture, int x, int y);

/*
 * Render LTexture at given point along with rotation angle, center point to rotate, and flipping type.
 */
extern void LTexture_RenderEx(LTexture* ltexture, int x, int y, float scale, double angle, SDL_Point* center, SDL_RendererFlip flip);

/*
 * Render clipped LTexture at the given point.
 */
extern void LTexture_ClippedRender(LTexture* texture, int x, int y, SDL_Rect* clip);

/*
 * Render clipped LTexture at the given point along with rotation angle, center point to rotate, and flipping type.
 */
extern void LTexture_ClippedRenderEx(LTexture* ltexture, int x, int y, float scale, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip);

/*
 * Set color modulation.
 */
extern void LTexture_SetColor(LTexture* ltexture, Uint8 red, Uint8 green, Uint8 blue);

/*
 * Set blending mode.
 */
extern void LTexture_SetBlendMode(LTexture* ltexture, SDL_BlendMode blending);

/*
 * Set alpha.
 */
extern void LTexture_SetAlpha(LTexture* ltexture, Uint8 alpha);

///
/// Lock texture
///
/// \param ltexture LTexture to lock
/// \return True if lock successfully, otherwise return false.
///
extern bool LTexture_LockTexture(LTexture* ltexture);

///
/// Unlock texture
///
/// \param ltexture LTexture to unlock
/// \return True if unlock successfully, otherwise return false.
///
extern bool LTexture_UnlockTexture(LTexture* ltexture);

///
/// Get pixel 32 bit (RGBA) from texture at specified position.
/// It will work only if texture has RGBA format with 8 bit each. If not, behavior is undefined.
///
/// \param ltexture LTexture got get pixel data at specified position from
/// \param x Position x to get pixel data
/// \param y Position y to get pixel data
///
extern Uint32 LTexture_GetPixel32(LTexture* ltexture, unsigned int x, unsigned int y);

/*
 * Free LTexture's resource.
 * After this call, texture will be NULL.
 */
extern void LTexture_Free(LTexture* texture);

#endif /* LTexture_h_ */
