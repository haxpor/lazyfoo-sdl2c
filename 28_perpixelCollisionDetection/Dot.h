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

  SDL_Rect colliders[11];  /* collision boxes */
};
typedef struct Dot Dot;

/// Initialize specified Dot
/// Use this in case when caller define Dot on stack.
/// This will start at position of x,y
extern void Dot_Init(Dot* dot, int x, int y, LTexture* texture);

/// update position of all colliders
extern void Dot_ShiftColliders(Dot* dot);

/// update Dot
/// Taking other colliders and number of it into account 
extern void Dot_Update(Dot* dot, float deltaTime);

/// update collision detection checking against other colliders
extern bool Dot_UpdateCollisions(Dot* dot, SDL_Rect* otherColliders, int numOtherColliders);

extern void Dot_Render(Dot* dot);

#endif /* Dot_h_ */
