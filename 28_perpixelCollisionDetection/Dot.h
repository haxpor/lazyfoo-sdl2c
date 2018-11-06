#ifndef Dot_h_
#define Dot_h_

#include "SDL.h"
#include "LTexture.h"

struct Dot {
  float posX;         /* position x */
  float posY;         /* position y */
  float velX;         /* velocity x */
  float velY;         /* velocity y */

  float targetVelX;   /* target of velocity x used in lerping */
  float targetVelY;   /* target of velocity y used in lerping */

  LTexture* texture;  /* pointer to texture */

  SDL_Rect collider;  /* collision box */
};
typedef struct Dot Dot;

#endif /* Dot_h_ */
