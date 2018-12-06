#include "Particle.h"
#include <stdlib.h>

static void init_default(Particle* p)
{
  p->x = 0;
  p->y = 0;
  p->velx = 0;
  p->vely = 0;
  p->accx = 0;
  p->accy = 0;
  p->frame = 0;
  p->scale = 1.0;
  p->lifetime = 0;
  p->original_lifetime = 0;
  p->is_dead = false;
  p->anim_timecount = 0;
}

Particle* Particle_new(int x, int y)
{
  Particle* out = malloc(sizeof(Particle));

  // initialize values
  if (!Particle_init(out, x, y))
  {
    // free memory now
    free(out);
    out = NULL;
  }

  return out;
}

bool Particle_init(Particle* p, int x, int y)
{
  init_default(p);

  p->x = x;
  p->y = y;

  return true;
}

void Particle_reset(Particle* p)
{
  // reset making this particle not dead
  p->x = 0;
  p->y = 0;
  p->velx = 0;
  p->vely = 0;
  p->accx = 0;
  p->accy = 0;
  p->frame = 0;
  p->scale = 1.0;
  p->lifetime = 0;
  p->original_lifetime = 0;
  p->is_dead = false;
  p->anim_timecount = 0;
}

void Particle_free_internals(Particle* p)
{
  // nothing to be done here
}

void Particle_free(Particle* p)
{
  Particle_free_internals(p);
  if (p != NULL)
  {
    free(p);
    p = NULL;
  }
}
