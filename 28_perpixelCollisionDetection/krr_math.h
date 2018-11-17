#ifndef krr_math_h_
#define krr_math_h_

#include "SDL.h"
#include <stdbool.h>

extern float krr_math_lerp(float a, float b, float t);

extern bool krr_math_checkCollision(SDL_Rect a, SDL_Rect b);

extern bool krr_math_checkCollisions(SDL_Rect *collidersA, int numCollidersA, SDL_Rect* collidersB, int numCollidersB);

#endif /* math_h_ */
