#ifndef ParticleGroup_h_
#define ParticleGroup_h_

#include "LTexture.h"
#include <stdbool.h>

///
/// Provide shared information & data for individual particle to operate
/// This reduces memory usage, and better able to control behavior of
/// particle in shared central way.
///
/// Note: We are not going to lerp the value throughout the lifetime of particle
/// for attributes listed via start../end.. as seen below. But it will only used at
/// the start, and when a particle's lifetime has ended. This is to keep performance
/// in good shape.
///
typedef struct {
  /// (read-only) texture used to render particles
  LTexture* texture;

  /// (read-only) rectangles to render animation
  SDL_Rect* anim_rects;

  /// (read-only) number of animation-rectangle
  int num_anim_rects;

  /// (read-only) managed internally to cache delay value for animation speed
  float anim_delay;

  /// mass of particle
  int start_particle_mass;
  int end_particle_mass;

  /// offset position x
  int start_particle_offsetx;
  int end_particle_offsetx;

  /// offset position y
  int start_particle_offsety;
  int end_particle_offsety;

  /// duration of lifetime for particle in seconds
  float start_particle_lifetime;
  float end_particle_lifetime;

  /// start/end of particle's velocity x
  float start_particle_velx;
  float end_particle_velx;

  // start/end of particle's velocity y
  float start_particle_vely;
  float end_particle_vely;

  // start/end of particle's acceleration x
  float start_particle_accx;
  float end_particle_accx;

  // start/end of particle's acceleration y
  float start_particle_accy;
  float end_particle_accy;

  // start/end of scale of particle
  float start_particle_scale;
  float end_particle_scale;

  // start/end of rotation angle of particle
  float start_particle_angle;
  float end_particle_angle;
} ParticleGroup;

///
/// Create a new ParticleGroup.
///
/// \param texture Texture used in rendering
/// \param frame_width Animation's frame width
/// \param frame_height Animation's frame height
/// \param anim_num_row Number of row for animation frame
/// \param anim_num_column Number of column for animation frames
/// \param anim_speed_fps Speed of animation in frame-per-second.
/// \return Newly created ParticleGroup is successfully created, otherwise return NULL.
///
extern ParticleGroup* ParticleGroup_new(LTexture* texture, int frame_width, int frame_height, int anim_num_row, int anim_num_column, int anim_speed_fps);

extern bool ParticleGroup_init(ParticleGroup* pg, LTexture* texture, int frame_width, int frame_height, int anim_num_row, int anim_num_column, int anim_speed_fps);

///
/// Free internals of ParticleGroup
///
/// \param pg ParticleGroup to be freed for its internals.
///
extern void ParticleGroup_free_internals(ParticleGroup* pg);

///
/// Free memory of ParticleGroup.
///
/// \param pg ParticleGroup to be freed.
///
extern void ParticleGroup_free(ParticleGroup* pg);

#endif
