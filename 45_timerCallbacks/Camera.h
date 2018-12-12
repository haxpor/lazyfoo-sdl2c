#ifndef Camera_h_
#define Camera_h_

#include "SDL.h"

typedef struct {
  SDL_Rect view_rect;

  float target_x;
  float target_y;

  float lerp_factor;
} Camera;

/// fill in default values for its properties
extern void Camera_init(Camera* cam, int x, int y, int view_rect_width, int view_rect_height);

/// update specified camera
/// this will lerp its position.
extern void Camera_update_lerpcenter(Camera* cam);

#endif
