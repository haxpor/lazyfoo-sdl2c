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
struct LTexture {
	SDL_Texture* texture;
	int width;
	int height;
};
typedef struct LTexture LTexture;

/*
 * Load texture at the specified path.
 * out will be filled with newly created LTexture.
 * colorKey.... is for color key for such texture
 * Return newly created LTexture as loaded from file.
 */
extern LTexture* LTexture_LoadFromFile(const char* path);
extern LTexture* LTexture_LoadFromFileWithColorKey(const char* path, Uint8 colorKeyRed, Uint8 colorKeyGreen, Uint8 colorKeyBlue);

#ifndef DISABLE_SDL_TTF_LIB
/*
 * Load texture from rendered text, and color.
 * out will be filled with newly created LTexture.
 * wrapLength is the length in pixel to do wrapping with newlines. Set to 0 to disable auto-wrapping.
 * Return newly created LTexture as loaded from rendered text.
 */
extern LTexture* LTexture_LoadFromRenderedText(const char* textureText, SDL_Color textColor, Uint32 wrapLength);
#endif

/*
 * Render LTexture at given point.
 */
extern void LTexture_Render(LTexture* texture, int x, int y);

/*
 * Render LTexture at given point along with rotation angle, center point to rotate, and flipping type.
 */
extern void LTexture_RenderEx(LTexture* ltexture, int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip);

/*
 * Render clipped LTexture at the given point.
 */
extern void LTexture_ClippedRender(LTexture* texture, int x, int y, SDL_Rect* clip);

/*
 * Render clipped LTexture at the given point along with rotation angle, center point to rotate, and flipping type.
 */
extern void LTexture_ClippedRenderEx(LTexture* ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip);

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

/*
 * Free LTexture's resource.
 * After this call, texture will be NULL.
 */
extern void LTexture_Free(LTexture* texture);

#endif /* LTexture_h_ */
