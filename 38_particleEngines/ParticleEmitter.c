#include "ParticleEmitter.h"
#include "krr_math.h"
#include "LTexture.h"

static void init_defaults(ParticleEmitter* emitter)
{
  emitter->particles = NULL;
  emitter->particlegroup = NULL;
  emitter->num_particles = 0;
  emitter->particle_update = NULL;
  emitter->x = 0;
  emitter->y = 0;
}

ParticleEmitter* ParticleEmitter_new(ParticleGroup* pg, int num_particles, int x, int y)
{
  ParticleEmitter* out = malloc(sizeof(ParticleGroup));
  
  // init default
  init_defaults(out);

  // init
  if (!ParticleEmitter_init(out, pg, num_particles, x, y))
  {
    // free memory immediately
    free(out);
    out = NULL;
  }

  return out;
}

bool ParticleEmitter_init(ParticleEmitter* emitter, ParticleGroup* pg, int num_particles, int x, int y)
{
  // set position to emitter
  emitter->x = x;
  emitter->y = y;

  // create particles according to input num_particles
  Particle *particles = malloc(sizeof(Particle) * num_particles);
  // loop through all particles in the pool to initialize its values
  // mostly randomly from what ParticleGroup has been configured
  for (int i=0; i<num_particles; i++)
  {
    // (we will render this relatively to position of particle emitter)
    // init particle
    Particle_init(particles + i, krr_math_rand_int2(pg->start_particle_offsetx, pg->end_particle_offsetx), krr_math_rand_int2(pg->start_particle_offsety, pg->end_particle_offsety));
    // set its other attributes randomly
    particles[i].mass = krr_math_rand_int2(pg->start_particle_mass, pg->end_particle_mass);
    particles[i].velx = krr_math_rand_float2(pg->start_particle_velx, pg->end_particle_velx);
    particles[i].vely = krr_math_rand_float2(pg->start_particle_vely, pg->end_particle_vely);
    particles[i].accx = krr_math_rand_float2(pg->start_particle_accx, pg->end_particle_accx);
    particles[i].accy = krr_math_rand_float2(pg->start_particle_accy, pg->end_particle_accy);
    particles[i].scale = krr_math_rand_float2(pg->start_particle_scale, pg->end_particle_scale);
    particles[i].original_lifetime = krr_math_rand_float2(pg->start_particle_lifetime, pg->end_particle_lifetime);
    particles[i].lifetime = particles[i].lifetime;
  }

  // set particles to emitter
  emitter->particles = particles;
  emitter->num_particles = num_particles;

  // set particlegroup
  emitter->particlegroup = pg;

  return true;
}

void ParticleEmitter_update(ParticleEmitter* emitter, float delta_time)
{
  ParticleGroup* pg = emitter->particlegroup;

  // temporary to hold particle in loop
  Particle* p = NULL;
  for(int i=0; i<emitter->num_particles; i++)
  {
    // get particle
    p = emitter->particles + i;

    // if particle is dead, then reset and re-random its attributes
    if (p->is_dead)
    {
      Particle_reset(p);

      p->x = krr_math_rand_int2(pg->start_particle_offsetx, pg->end_particle_offsetx);
      p->y = krr_math_rand_int2(pg->start_particle_offsety, pg->end_particle_offsety);
      p->mass = krr_math_rand_int2(pg->start_particle_mass, pg->end_particle_mass);
      p->velx = krr_math_rand_float2(pg->start_particle_velx, pg->end_particle_velx);
      p->vely = krr_math_rand_float2(pg->start_particle_vely, pg->end_particle_vely);
      p->accx = krr_math_rand_float2(pg->start_particle_accx, pg->end_particle_accx);
      p->accy = krr_math_rand_float2(pg->start_particle_accy, pg->end_particle_accy);
      p->scale = krr_math_rand_float2(pg->start_particle_scale, pg->end_particle_scale);
      p->original_lifetime = krr_math_rand_float2(pg->start_particle_lifetime, pg->end_particle_lifetime);     
      p->lifetime = p->original_lifetime;
    }
    // otherwise, update its position and age
    else
    {
      // chose to apply delta_time with velocity when additioned to position
      // to avoid having too small value of acceleration
      p->velx += p->accx;
      p->vely += p->accy;
      p->x += p->velx * delta_time;
      p->y += p->vely * delta_time;

      // update accumulated time
      p->anim_timecount += delta_time;
      if (p->anim_timecount >= pg->anim_delay)
      {
        // increment to next frame
        p->frame = (p->frame+1) % pg->num_anim_rects;
        // reset animation timecount
        p->anim_timecount -= pg->anim_delay;
      }

      // decrease lifetime of particle
      p->lifetime -= delta_time;
      if (p->lifetime <= 0)
      {
        p->is_dead = true;
      }
    }
  }
}

void ParticleEmitter_apply_force(ParticleEmitter* emitter, int force_x, int force_y)
{
  Particle* p = NULL;

  for (int i=0; i<emitter->num_particles; i++)
  {
    // get particle
    p = emitter->particles + i;

    // if particle is not dead yet, then apply force
    if (!p->is_dead)
    {
      p->accx += force_x / p->mass;
      p->accy += force_y / p->mass;
    }
  }
}

void ParticleEmitter_render(ParticleEmitter* emitter)
{
  ParticleGroup* pg = emitter->particlegroup;
  LTexture* texture = pg->texture;
  SDL_Rect* anim_rects = pg->anim_rects;

  // to hold particle temporarily
  Particle* p = NULL;
  // set blend mode
  // as we will render particles's alpha according to its current age
  SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_BLEND);

  for (int i=0; i<emitter->num_particles; i++)
  {
    // get particle
    p = emitter->particles + i;

    // if particle not dead yet
    if (!p->is_dead)
    {
      // set alpha value according to its current age
      SDL_SetTextureAlphaMod(texture->texture, (int)(p->lifetime / p->original_lifetime * 255));

      // render current frame
      LTexture_ClippedRenderEx(texture, emitter->x - p->x, emitter->y - p->y, p->scale, &anim_rects[p->frame], 0, NULL, SDL_FLIP_NONE);
    }
  }

  // set blend mode back to normal
  SDL_SetTextureBlendMode(texture->texture, SDL_BLENDMODE_NONE);
}

void ParticleEmitter_free_internals(ParticleEmitter* emitter)
{
  if (emitter != NULL)
  {
    // free all particles
    if (emitter->particles != NULL)
    {
      // loop through all particles to free it
      for (int i=0; i<emitter->num_particles; i++)
      {
        Particle_free_internals(emitter->particles + i);
      }
      
      free(emitter->particles);
      emitter->particles = NULL;
    }

    // reset update function
    emitter->particle_update = NULL;
  }
}

void ParticleEmitter_free(ParticleEmitter* emitter)
{
  ParticleEmitter_free_internals(emitter);

  if (emitter != NULL)
  {
    free(emitter);
    emitter = NULL; 
  }
}
