#include "Camera.h"
#include "krr_math.h"

#define LERP_FACTOR 0.07

void Camera_init(Camera* cam, int x, int y, int view_rect_width, int view_rect_height)
{
  cam->view_rect.x = x;
  cam->view_rect.y = y;
  cam->view_rect.w = view_rect_width;
  cam->view_rect.h = view_rect_height;

  cam->target_x = 0;
  cam->target_y = 0;
  cam->lerp_factor = LERP_FACTOR;
}

void Camera_update_lerpcenter(Camera* cam)
{
  // lerp position on velocity
  cam->view_rect.x = krr_math_lerp(cam->view_rect.x, cam->target_x, cam->lerp_factor);
  cam->view_rect.y = krr_math_lerp(cam->view_rect.y, cam->target_y, cam->lerp_factor);
}
