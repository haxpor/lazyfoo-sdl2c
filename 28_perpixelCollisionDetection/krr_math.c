#include "krr_math.h"

float krr_math_lerp(float a, float b, float t)
{
  return a + (b-a)*t;
}

bool krr_math_checkCollision(SDL_Rect a, SDL_Rect b)
{
  if (a.x + a.w > b.x &&
      a.y + a.h > b.y &&
      b.x + b.w > a.x &&
      b.y + b.h > a.y)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool krr_math_checkCollisions(SDL_Rect* collidersA, int numCollidersA, SDL_Rect* collidersB, int numCollidersB)
{
  SDL_Rect rectA;
  SDL_Rect rectB;

  // go through each box
  for (int i=0; i<numCollidersA; i++)
  {
    rectA = collidersA[i];

    for (int j=0; j<numCollidersB; j++)
    {
      rectB = collidersB[j];

      // not call checkCollision() for better performance
      if (rectA.x + rectA.w > rectB.x &&
          rectA.y + rectA.h > rectB.y &&
          rectB.x + rectB.w > rectA.x &&
          rectB.y + rectB.h > rectA.y)
      {
        return true;
      } 
    }
  }

  return false;
}
