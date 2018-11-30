#ifndef camera_h_
#define camera_h_

#include "SDL.h"

typedef struct {
  SDL_Rect view_rect;

  float target_x;
  float target_y;

  float lerp_factor;
} camera;

/// fill in default values for its properties
extern void camera_init(camera* cam, int x, int y, int view_rect_width, int view_rect_height);

/// update specified camera
/// this will lerp its position.
extern void camera_update_lerpcenter(camera* cam);

#endif
