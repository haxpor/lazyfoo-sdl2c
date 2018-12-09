#include "bound_sys.h"

static int bound_width;
static int bound_height;

void BoundSystem_source_init(int arg_bound_width, int arg_bound_height)
{
  bound_width = arg_bound_width;
  bound_height = arg_bound_height;
}

bool BoundSystem_global_screenbound_Dot(Dot* dot)
{
  bool collided = false;

  if (dot->posX < 0 || dot->collider.x + dot->collider.r > bound_width)
  {
    // move back
    dot->posX -= dot->velX;
    Dot_ShiftCollider(dot);

    collided = true;
  }

  if (dot->posY < 0 || dot->collider.y + dot->collider.r > bound_height)
  {
    // move back
    dot->posY -= dot->velY;
    Dot_ShiftCollider(dot);

    collided = true;
  }

  return collided;
}

bool BoundSystem_bound_Camera(Camera* cam)
{
  bool collided = false;

  SDL_Rect view_rect = cam->view_rect;

  if (view_rect.x < 0)
  {
    cam->view_rect.x = 0;
    collided = true;
  }
  else if (view_rect.x > bound_width - view_rect.w)
  {
    cam->view_rect.x = bound_width - view_rect.w;
    collided = true;
  }

  if (view_rect.y < 0)
  {
    cam->view_rect.y = 0;
    collided = true;
  }
  else if (view_rect.y > bound_height - view_rect.h)
  {
    cam->view_rect.y = bound_height - view_rect.h;
    collided = true;
  }

  return collided;
}
