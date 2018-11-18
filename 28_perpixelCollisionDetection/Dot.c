#include "Dot.h"
#include "common.h"
#include "common_debug.h"
#include "krr_math.h"

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

  // set rough collider to be the same size of of texture
  // this will be used internally before checking fine-grained collision checking with
  // colliders
  dot->roughCollider.w = dot->texture->width;
  dot->roughCollider.h = dot->texture->height;
  
  // set fine-grain detail of colliders of Dot
  // take a look at dot.bmp and inspect rectangles that could form as dot as a result
  // we will have 11 rectangles forming together as fine-grain rectangle colliders
  //
  // we will specify only for its width and height as in run-time through Dot_ShiftColliders()
  // it will calculate new position based on the center of Dot itself
  dot->colliders[0].w = 6;
  dot->colliders[0].h = 1;

  dot->colliders[1].w = 10;
  dot->colliders[1].h = 1;

  dot->colliders[2].w = 14;
  dot->colliders[2].h = 1;

  dot->colliders[3].w = 16;
  dot->colliders[3].h = 2;

  dot->colliders[4].w = 18;
  dot->colliders[4].h = 2;

  dot->colliders[5].w = 20;
  dot->colliders[5].h = 6;

  dot->colliders[6].w = 18;
  dot->colliders[6].h = 2;

  dot->colliders[7].w = 16;
  dot->colliders[7].h = 2;

  dot->colliders[8].w = 14;
  dot->colliders[8].h = 1;

  dot->colliders[9].w = 10;
  dot->colliders[9].h = 1;

  dot->colliders[10].w = 6;
  dot->colliders[10].h = 1;
}

void Dot_Update(Dot* dot, float deltaTime)
{
  // lerp on velocity
  dot->velX = krr_math_lerp(dot->velX, dot->targetVelX, 0.07);
  dot->velY = krr_math_lerp(dot->velY, dot->targetVelY, 0.07);
  // calculate final position
  dot->posX += dot->velX;
  dot->posY += dot->velY;
  Dot_ShiftColliders(dot);
}

bool Dot_UpdateCollision(Dot* dot, const SDL_Rect* const otherRoughCollider)
{
  assert(otherRoughCollider != NULL);

  // result of collision
  bool collided = false;
  
  if (krr_math_checkCollision(dot->roughCollider, *otherRoughCollider))
  {
    // move back
    dot->posX -= dot->velX;
    Dot_ShiftColliders(dot);

    collided = true;
  }
  if (krr_math_checkCollision(dot->roughCollider, *otherRoughCollider))
  {
    // move back
    dot->posY -= dot->velY;
    Dot_ShiftColliders(dot);

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
  
  if (krr_math_checkCollision(dot->roughCollider, *otherRoughCollider) && otherColliders != NULL && krr_math_checkCollisions(dot->colliders, 11, otherColliders, numOtherColliders))
  {
    // move back
    dot->posX -= dot->velX;
    Dot_ShiftColliders(dot);

    collided = true;
  }
  if (krr_math_checkCollision(dot->roughCollider, *otherRoughCollider) && otherColliders != NULL && krr_math_checkCollisions(dot->colliders, 11, otherColliders, numOtherColliders))
  {
    // move back
    dot->posY -= dot->velY;
    Dot_ShiftColliders(dot);

    collided = true;
  }

  return collided;
}

void Dot_ShiftColliders(Dot* dot)
{
  // shift rough collider
  dot->roughCollider.x = dot->posX + (dot->texture->width - dot->roughCollider.w) / 2;
  dot->roughCollider.y = dot->posY;

  // the row offset
  int r = 0;

  // go through the dot's collision boxes
  for (int set = 0; set < 11; ++set)
  {
    // center the collision box
    dot->colliders[set].x = dot->posX + (dot->texture->width - dot->colliders[set].w) / 2;
    
    // set the collision box at its row offset
    dot->colliders[set].y = dot->posY + r;

    // move the row offset down the height of the collision box
    r += dot->colliders[set].h;
  }
}

void Dot_Render(Dot* dot)
{
  LTexture_Render(dot->texture, dot->posX, dot->posY);
}
