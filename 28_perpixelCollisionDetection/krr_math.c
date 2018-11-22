#include "krr_math.h"

float krr_math_lerp(float a, float b, float t)
{
  return a + (b-a)*t;
}

bool krr_math_checkCollision(SDL_Rect a, SDL_Rect b, int* deltaCollisionX, int* deltaCollisionY)
{
  if (a.x + a.w > b.x &&
      a.y + a.h > b.y &&
      b.x + b.w > a.x &&
      b.y + b.h > a.y)
  {
    // calculate delta collision distance 
    // calculate based from a to b
    if (deltaCollisionX != NULL) {
      if (a.x + a.w > b.x && a.x + a.w - b.x < a.w/2) {
        *deltaCollisionX = a.x + a.w - b.x;
      }
      // opposite direction
      else if (b.x + b.w > a.x && b.x + b.w - a.x < a.w/2) {
        *deltaCollisionX = (b.x + b.w - a.x) * -1;
      }
    }
    
    // same as above, from a to b
    if (deltaCollisionY != NULL) {
      if (a.y + a.h > b.y && a.y + a.h - b.y < a.h/2) {
        *deltaCollisionY = a.y + a.h - b.y;
      }
      else if (b.y + b.h > a.y && b.y + b.h - a.y < a.h/2) {
        *deltaCollisionY = (b.y + b.h - a.y) * -1;
      }
    }
    return true;
  }
  else
  {
    // just ignore delta collision
    return false;
  }
}

bool krr_math_checkCollisions(SDL_Rect* collidersA, int numCollidersA, SDL_Rect* collidersB, int numCollidersB, int* deltaCollisionX, int* deltaCollisionY)
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
        // calculate delta collision distance 
        // calculate based from a to b
        if (deltaCollisionX != NULL) {
          if (rectA.x + rectA.w > rectB.x && rectA.x + rectA.w - rectB.x < rectA.w/2) {
            *deltaCollisionX = rectA.x + rectA.w - rectB.x;
          }
          // opposite direction
          else if (rectB.x + rectB.w > rectA.x && rectB.x + rectB.w - rectA.x < rectA.w/2) {
            *deltaCollisionX = (rectB.x + rectB.w - rectA.x) * -1;
          }
        }
        
        // same as above, from a to b
        if (deltaCollisionY != NULL) {
          if (rectA.y + rectA.h > rectB.y && rectA.y + rectA.h - rectB.y < rectA.h/2) {
            *deltaCollisionY = rectA.y + rectA.h - rectB.y;
          }
          else if (rectB.y + rectB.h > rectA.y && rectB.y + rectB.h - rectA.y < rectA.h/2) {
            *deltaCollisionY = (rectB.y + rectB.h - rectA.y) * -1;
          }
        }
        return true;
      } 
    }
  }

  return false;
}
