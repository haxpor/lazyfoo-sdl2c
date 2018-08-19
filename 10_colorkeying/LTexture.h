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

// load image at specified path
extern LTexture* LTexture_LoadFromFile(const char* path);

// render texture at given point
extern void LTexture_Render(LTexture* texture, int x, int y);

// free texture
extern void LTexture_Free(LTexture* texture);

#endif /* LTexture_h_ */
