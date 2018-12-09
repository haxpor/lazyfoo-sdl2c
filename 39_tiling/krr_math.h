#ifndef krr_math_h_
#define krr_math_h_

#include "SDL.h"
#include <stdbool.h>
#include "Circle.h"

extern float krr_math_lerp(float a, float b, float t);

/// Seed rand() function with current time via time(NULL).
extern void krr_math_rand_seed_time();

/// Seed rand() with seed
/// \param seed Number to be set as seed for rand()
extern void krr_math_rand_seed(unsigned int seed);

/// Random integer from [0, max]).
/// \param max Maximum number 
/// \return Randomized number in range [0, max]
extern int krr_math_rand_int(int max);

/// Random integer from [min, max].
/// \param inclusive_min Minimum number for result
/// \param exclusive_max Maximum number for result
/// \return Randomized number in range [min, max].
extern int krr_math_rand_int2(int min, int max);

/// Random float number from [0, max].
/// \param max Maximum float number
/// \return Randomized float number in range [0, max].
extern float krr_math_rand_float(float max);

/// Radom float number from [min, max].
/// \param min Minimum number for result
/// \param max Maximum number for result
/// \return Randomized number in range [min, max].
extern float krr_math_rand_float2(float min, float max);

extern bool krr_math_checkCollision(SDL_Rect a, SDL_Rect b, int* deltaCollisionX, int* deltaCollisionY);

extern bool krr_math_checkCollisions(SDL_Rect *collidersA, int numCollidersA, SDL_Rect* collidersB, int numCollidersB, int* deltaCollisionX, int* deltaCollisionY);

extern bool krr_math_checkCollision_cc(Circle a, Circle b, int* deltaCollisionX, int* deltaCollisionY);

extern bool krr_math_checkCollision_cr(Circle colliderA, SDL_Rect colliderB, int* deltaCollisionX, int* deltaCollisionY);

extern bool krr_math_checkCollision_cr_arr(Circle colliderA, SDL_Rect* collidersB, int numCollidersB, int* deltaCollisionX, int* deltaCollisionY);

#endif /* math_h_ */
