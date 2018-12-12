#ifndef bound_sys_h_
#define bound_sys_h_

#include "Dot.h"
#include "Camera.h"

// screenbounder system
typedef struct {
  bool (*f)(Dot*);  
} BoundSystem;

void BoundSystem_source_init(int bound_width, int bound_height);
bool BoundSystem_global_screenbound_Dot(Dot* dot);
bool BoundSystem_bound_Camera(Camera* cam);

#endif
