#ifndef Dot_h_
#define Dot_h_

#include "SDL.h"
#include "LTexture.h"
#include "Circle.h"
#include "Particle.h"

#define TOTAL_PARTICLES 20

struct Dot {
  float posX;         /* position x */
  float posY;         /* position y */
  float velX;         /* velocity x */
  float velY;         /* velocity y */

  float targetVelX;   /* target of velocity x used in lerping */
  float targetVelY;   /* target of velocity y used in lerping */

  LTexture* texture;  /* pointer to texture */

  Circle collider;  /* collision box as circle */

  Particle particles[TOTAL_PARTICLES];  /* particles */
};
typedef struct Dot Dot;

/// Initialize specified Dot
/// Use this in case when caller define Dot on stack.
/// This will start at position of x,y
extern void Dot_Init(Dot* dot, int x, int y, LTexture* texture);

/// update position of all colliders
extern void Dot_ShiftCollider(Dot* dot);

/// update Dot
/// Taking other colliders and number of it into account 
extern void Dot_Update(Dot* dot, float deltaTime);

/// update collision just against another rough collider
/// otherRoughCollider - another rough collider to check; cannot be NULL
extern bool Dot_UpdateCollision(Dot* dot, const SDL_Rect* const otherRoughCollider);

/// update collision detection checking against other collider as Circle type.
/// dot - Dot to base collision against
/// otherCollider - other collider of type Circle to check for collision
/// return true if collided, otherwise return false
extern bool Dot_UpdateCollision_Circle(Dot* dot, const Circle* const otherCollider);

/// update collision detection checking against other colliders
/// otherColliders - fine-grained colliders; it can be NULL. If so then no fine-grained collision checking will be checked.
/// numOtherColliders - number of colliders to check for fine-grained colliders
/// roughCollider - is the rough collider before checking more fine-grained collisions, it can be NULL; if so then it will check only with otherColliders
///
/// Note: otherRoughCollider cannot be NULL. If use this function, at least caller need to know rough collider prior to the call.
extern bool Dot_UpdateCollisions(Dot* dot, SDL_Rect* otherColliders, int numOtherColliders, const SDL_Rect* const otherRoughCollider);

extern void Dot_Render(Dot* dot);

/// render Dot base on camera's position
extern void Dot_Render_w_camera(Dot* dot, int cam_pos_x, int cam_pos_y);

#endif /* Dot_h_ */
