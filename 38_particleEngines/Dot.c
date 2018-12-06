#include "Dot.h"
#include "common.h"
#include "common_debug.h"
#include "krr_math.h"

static void render_particles(Dot* dot)
{

}

void Dot_Init(Dot* dot, int x, int y, LTexture* texture)
{
  // set the initial position of dot
  dot->posX = x;
  dot->posY = y;

  // set initial velocity
  dot->velX = 0;
  dot->velY = 0;
  // set initial target velocity
  dot->targetVelX = 0;
  dot->targetVelY = 0;

  // set loaded texture (from elsewhere)
  dot->texture = texture;

  // width and height is the same size
  int halfSize = dot->texture->width/2;
  dot->collider.x = x + halfSize;
  dot->collider.y = y + halfSize;
  dot->collider.r = halfSize;
}

void Dot_Update(Dot* dot, float deltaTime)
{
  // lerp on velocity
  dot->velX = krr_math_lerp(dot->velX, dot->targetVelX, 0.07);
  dot->velY = krr_math_lerp(dot->velY, dot->targetVelY, 0.07);
  // calculate final position
  dot->posX += dot->velX;
  dot->posY += dot->velY;
  Dot_ShiftCollider(dot);
}

bool Dot_UpdateCollision(Dot* dot, const SDL_Rect* const otherRoughCollider)
{
  assert(otherRoughCollider != NULL);

  // result of collision
  bool collided = false;

  int deltaCollisionX = 0;
  int deltaCollisionY = 0;
  
  if (krr_math_checkCollision_cr(dot->collider, *otherRoughCollider, &deltaCollisionX, &deltaCollisionY))
  {
    // move back both axis
    dot->posX -= deltaCollisionX;
    dot->posY -= deltaCollisionY;
    Dot_ShiftCollider(dot);

    collided = true;
  }

  return collided;
}

bool Dot_UpdateCollisions(Dot* dot, SDL_Rect* otherColliders, int numOtherColliders, const SDL_Rect* const otherRoughCollider)
{
  assert(otherRoughCollider != NULL);
  assert(otherColliders != NULL);
  assert(numOtherColliders > 0);

  // result of collision
  bool collided = false;

  int deltaCollisionX = 0;
  int deltaCollisionY = 0;
  
  if (krr_math_checkCollision_cr_arr(dot->collider, otherColliders, numOtherColliders, &deltaCollisionX, &deltaCollisionY))
  {
    // move back both axis
    dot->posX -= deltaCollisionX;
    dot->posY -= deltaCollisionY;
    Dot_ShiftCollider(dot);

    collided = true;
  }

  return collided;
}

bool Dot_UpdateCollision_Circle(Dot* dot, const Circle* const otherCollider)
{
  assert(dot != NULL);
  assert(otherCollider != NULL);

  // result of collision
  bool collided = false;

  int deltaCollisionX = 0;
  int deltaCollisionY = 0;
  
  if (krr_math_checkCollision_cc(dot->collider, *otherCollider, &deltaCollisionX, &deltaCollisionY))
  {
    // move back both axis
    dot->posX -= deltaCollisionX;
    dot->posY -= deltaCollisionY;
    Dot_ShiftCollider(dot);

    collided = true;
  }

  return collided;
}

void Dot_ShiftCollider(Dot* dot)
{
  dot->collider.x = dot->posX + dot->collider.r;
  dot->collider.y = dot->posY + dot->collider.r;
}

void Dot_Render(Dot* dot)
{
  LTexture_Render(dot->texture, dot->posX, dot->posY);
}

void Dot_Render_w_camera(Dot* dot, int cam_pos_x, int cam_pos_y)
{
  LTexture_Render(dot->texture, dot->posX - cam_pos_x, dot->posY - cam_pos_y);
}
