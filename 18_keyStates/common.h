/*
 * Contains common variables used across multiple source files.
 */

#ifndef common_h_
#define common_h_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

// CAUTION: It's user's resposibility to define these variables before use.

/*
 * The window we'll be rendering to.
 */
extern SDL_Window* gWindow;

/*
 * The window's renderer.
 */
extern SDL_Renderer* gRenderer;

/*
 * Font used in the app.
 * This is the main one.
 */
extern TTF_Font *gFont;

#endif /* common_h_ */
