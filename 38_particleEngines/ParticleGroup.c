#include "ParticleGroup.h"
#include <stdlib.h>

static void init_defaults(ParticleGroup* pg)
{
  pg->texture = NULL;
  pg->anim_rects = NULL;
  pg->num_anim_rects = 0;
  pg->anim_delay = 0;

  // default of particle's configuations as follows
  // users are freely to configure them
  pg->start_particle_offsetx = 5;
  pg->end_particle_offsetx = 7;

  pg->start_particle_offsety = 5;
  pg->end_particle_offsety = 7;

  pg->start_particle_lifetime = 3.0;
  pg->end_particle_lifetime = 5.0;

  pg->start_particle_velx = 0.0;
  pg->end_particle_velx = 0.0;

  pg->start_particle_vely = 500.0;
  pg->end_particle_vely = 600.0;

  pg->start_particle_accx = 0.0001;
  pg->end_particle_accx = 0.000011;

  pg->start_particle_accy = 0.0001;
  pg->end_particle_accy = 0.000011;

  pg->start_particle_scale = 1.0;
  pg->end_particle_scale = 1.2;

  pg->start_particle_angle = 0.0;
  pg->end_particle_angle = 0.0;
}

ParticleGroup* ParticleGroup_new(LTexture* texture, int frame_width, int frame_height, int anim_num_row, int anim_num_column, int anim_speed_fps)
{
  ParticleGroup* out = malloc(sizeof(ParticleGroup));
  // init defaults
  init_defaults(out);

  if (!ParticleGroup_init(out, texture, frame_width, frame_height, anim_num_row, anim_num_column, anim_speed_fps))
  {
    // free memory immediately
    free(out);
    out = NULL;
  }

  return out;
}

bool ParticleGroup_init(ParticleGroup* pg, LTexture* texture, int frame_width, int frame_height, int anim_num_row, int anim_num_column, int anim_speed_fps)
{
  pg->texture = texture;

  // compute for all SDL_Rects for animation according to input of frame width/height for number of rows/columns
  pg->num_anim_rects = anim_num_row * anim_num_column;
  // allocate enough memory space for anim_rects
  pg->anim_rects = malloc(sizeof(SDL_Rect) * pg->num_anim_rects);
  
  for (int i=0; i<anim_num_row; i++)
  {
    for (int j=0; j<anim_num_column; j++)
    {
      SDL_Rect r = {frame_width*j, frame_height*i, frame_width, frame_height};
      pg->anim_rects[j + i*anim_num_column] = r;
    }
  }

  // cache anim delay for how long we should wait to render 1 frame per second
  pg->anim_delay = 1.0 / anim_speed_fps;

  return true;
}

void ParticleGroup_free_internals(ParticleGroup* pg)
{
  if (pg->anim_rects != NULL)
  {
    free(pg->anim_rects);
    pg->anim_rects = NULL;
  }

  pg->num_anim_rects = 0;
}

void ParticleGroup_free(ParticleGroup* pg)
{
  // free internals first
  ParticleGroup_free_internals(pg);

  // free
  if (pg != NULL)
  {
    free(pg);
    pg = NULL;
  }
}
