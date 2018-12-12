/*
 * Contains common variables used across multiple source files.
 */

#ifndef common_h_
#define common_h_

#include "SDL.h"
#ifndef DISABLE_SDL_TTF_LIB
#include "SDL_ttf.h"
#endif
#include <stdbool.h>
#include "LWindow.h"

// CAUTION: It's user's resposibility to define these variables before use.

/*
 * The window we'll be rendering to.
 */
extern LWindow* gWindow;

#ifndef DISABLE_SDL_TTF_LIB
/*
 * Font used in the app.
 * This is the main one.
 */
extern TTF_Font *gFont;
#endif

extern float common_frameTime;
#ifndef DISABLE_FPS_CALC
/*
 * Variables used in calculate frame rate per second.
 */
extern int common_frameCount;
extern float common_frameAccumTime;
extern float common_avgFPS;
#endif

#endif /* common_h_ */
