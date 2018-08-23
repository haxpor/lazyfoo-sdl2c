/*
 * LTexture
 *
 * It represents a texture wrapper with functionality related to it.
 */

#ifndef LTexture_h_
#define LTexture_h_

#include <stdbool.h>
#include <SDL2/SDL_image.h>

// i know guys, we will care about byte alignment of struct later ;)
struct LTexture {
	SDL_Texture* texture;
	int width;
	int height;
};
typedef struct LTexture LTexture;

// load image at specified path (created on stack)
// then return resulted via out.
// Return true if loading succeeds, otherwise return false.
extern bool LTexture_LoadFromFile(const char* path, LTexture* out);

// render texture at given point
extern void LTexture_Render(LTexture* texture, int x, int y);

// render clipped texture at the given point
extern void LTexture_ClippedRender(LTexture* texture, int x, int y, SDL_Rect* clip);

// set color modulation
extern void LTexture_SetColor(LTexture* ltexture, Uint8 red, Uint8 green, Uint8 blue);

// clear internal resource as used by LTexture
// caller still need to free() LTexture which is texture in this case if it was dynamically created on heap
extern void LTexture_Free(LTexture* texture);

#endif /* LTexture_h_ */
