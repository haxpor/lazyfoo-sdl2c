#include "krr_math.h"
#include <stdlib.h>
#include <time.h>

float krr_math_lerp(float a, float b, float t)
{
  return a + (b-a)*t;
}

void krr_math_rand_seed_time()
{
  // set seed from current time
  srand(time(NULL));
}

void krr_math_rand_seed(unsigned int seed)
{
  // set seed from specified seed number
  srand(seed);
}

int krr_math_rand_int(int max)
{
  // FIXME: this is POOR approach, see http://c-faq.com/lib/randrange.html
  return rand() % (max+1);
}

int krr_math_rand_int2(int min, int max)
{
  // FIXME: this is POOR approach, see http://c-faq.com/lib/randrange.html
  return rand() % (max+1-min) + min;
}

float krr_math_rand_float(float max)
{
  return (float)((double)rand() / ((double)RAND_MAX + 1) * (max+1));
}

float krr_math_rand_float2(float min, float max)
{
  return (float)((double)rand() / ((double)RAND_MAX + 1) * (max+1-min) + min);
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

bool krr_math_checkCollision_cc(Circle a, Circle b, int* deltaCollisionX, int* deltaCollisionY)
{
  if (a.x + a.r > b.x - b.r &&
      a.y + a.r > b.y - b.r &&
      b.x + b.r > a.x - a.r &&
      b.y + b.r > a.y - a.r)
  {
    // calculate delta collision distance 
    // calculate based from a to b
    if (deltaCollisionX != NULL) {
      if (a.x + a.r > b.x - b.r && a.x + a.r - (b.x - b.r) < a.r/2) {
        *deltaCollisionX = a.x + a.r - (b.x - b.r);
      }
      // opposite direction
      else if (b.x + b.r > a.x - a.r && b.x + b.r - (a.x - a.r) < a.r/2) {
        *deltaCollisionX = (b.x + b.r - (a.x - a.r)) * -1;
      }
    }
    
    // same as above, from a to b
    if (deltaCollisionY != NULL) {
      if (a.y + a.r > b.y - b.r && a.y + a.r - (b.y - b.r) < a.r/2) {
        *deltaCollisionY = a.y + a.r - (b.y - b.r);
      }
      // opposite direction
      else if (b.y + b.r > a.y - a.r && b.y + b.r - (a.y - a.r) < a.r/2) {
        *deltaCollisionY = (b.y + b.r - (a.y - a.r)) * -1;
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

bool krr_math_checkCollision_cr(Circle colliderA, SDL_Rect colliderB, int* deltaCollisionX, int* deltaCollisionY)
{
  // not call checkCollision() for better performance
  if (colliderA.x + colliderA.r > colliderB.x &&
      colliderA.y + colliderA.r > colliderB.y &&
      colliderB.x + colliderB.w > colliderA.x - colliderA.r &&
      colliderB.y + colliderB.h > colliderA.y - colliderA.r)
  {
    // calculate delta collision distance 
    // calculate based from a to b
    if (deltaCollisionX != NULL) {
      if (colliderA.x + colliderA.r > colliderB.x && colliderA.x + colliderA.r - colliderB.x < colliderA.r/2) {
        *deltaCollisionX = colliderA.x + colliderA.r - colliderB.x;
      }
      // opposite direction
      else if (colliderB.x + colliderB.w > colliderA.x - colliderA.r && colliderB.x + colliderB.w - (colliderA.x - colliderA.r) < colliderA.r/2) {
        *deltaCollisionX = (colliderB.x + colliderB.w - (colliderA.x - colliderA.r)) * -1;
      }
    }
    
    // same as above, from a to b
    if (deltaCollisionY != NULL) {
      if (colliderA.y + colliderA.r > colliderB.y && colliderA.y + colliderA.r - colliderB.y < colliderA.r/2) {
        *deltaCollisionY = colliderA.y + colliderA.r - colliderB.y;
      }
      else if (colliderB.y + colliderB.h > colliderA.y - colliderA.r && colliderB.y + colliderB.h - (colliderA.y - colliderA.r) < colliderA.r/2) {
        *deltaCollisionY = (colliderB.y + colliderB.h - (colliderA.y - colliderA.r)) * -1;
      }
    }
    return true;
  } 
  else
  {
    return false;
  }
}

bool krr_math_checkCollision_cr_arr(Circle colliderA, SDL_Rect* collidersB, int numCollidersB, int* deltaCollisionX, int* deltaCollisionY)
{
  SDL_Rect rectB;

  for (int j=0; j<numCollidersB; j++)
  {
    rectB = collidersB[j];

    // not call checkCollision() for better performance
    if (colliderA.x + colliderA.r > rectB.x &&
        colliderA.y + colliderA.r > rectB.y &&
        rectB.x + rectB.w > colliderA.x - colliderA.r &&
        rectB.y + rectB.h > colliderA.y - colliderA.r)
    {
      // calculate delta collision distance 
      // calculate based from a to b
      if (deltaCollisionX != NULL) {
        if (colliderA.x + colliderA.r > rectB.x && colliderA.x + colliderA.r - rectB.x < colliderA.r/2) {
          *deltaCollisionX = colliderA.x + colliderA.r - rectB.x;
        }
        // opposite direction
        else if (rectB.x + rectB.w > colliderA.x - colliderA.r && rectB.x + rectB.w - (colliderA.x - colliderA.r) < colliderA.r/2) {
          *deltaCollisionX = (rectB.x + rectB.w - (colliderA.x - colliderA.r)) * -1;
        }
      }
      
      // same as above, from a to b
      if (deltaCollisionY != NULL) {
        if (colliderA.y + colliderA.r > rectB.y && colliderA.y + colliderA.r - rectB.y < colliderA.r/2) {
          *deltaCollisionY = colliderA.y + colliderA.r - rectB.y;
        }
        else if (rectB.y + rectB.h > colliderA.y - colliderA.r && rectB.y + rectB.h - (colliderA.y - colliderA.r) < colliderA.r/2) {
          *deltaCollisionY = (rectB.y + rectB.h - (colliderA.y - colliderA.r)) * -1;
        }
      }
      return true;
    } 
  }

  return false;
}
