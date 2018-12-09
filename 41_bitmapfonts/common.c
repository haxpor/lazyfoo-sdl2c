#include "common.h"

LWindow* gWindow = NULL;

#ifndef DISABLE_SDL_TTF_LIB
TTF_Font* gFont = NULL;
#endif

float common_frameTime = 0.0f;
#ifndef DISABLE_FPS_CALC
int common_frameCount = 0;
float common_frameAccumTime = 0.0f;
float common_avgFPS = 0.0f;
#endif
