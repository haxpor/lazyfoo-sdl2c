/*
 * Contains common variables used across multiple source files.
 */

#ifndef common_h_
#define common_h_

#include <SDL2/SDL.h>
#include <stdbool.h>

// CAUTION: It's user's resposibility to define these variables before use.

// the window we'll be rendering to
extern SDL_Window* gWindow;

// the window's renderer
extern SDL_Renderer* gRenderer;

#endif /* common_h_ */
