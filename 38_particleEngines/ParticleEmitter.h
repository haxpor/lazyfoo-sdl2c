#ifndef ParticleEmitter_h_
#define ParticleEmitter_h_

#include "Particle.h"
#include "ParticleGroup.h"

///
/// ParticleEmitter is the manager for similar type of Particle
/// for updating, rendering, and its behavior.
///
typedef struct {
  /// (read-only) pointer to all particles in management, internally managed.
  Particle* particles;

  /// (read-only) particle group acts as info & data for emitter
  ParticleGroup* particlegroup;

  /// (read-only) number of particles
  int num_particles;

  /// position x
  int x;

  /// position y
  int y;

  /// update for individual Particle (not used)
  /// It will be set to default update function by default which has default behavior.
  /// User can set this to custom update function to achieve different behavior of emitter.
  void (*particle_update)(float delta_time);
} ParticleEmitter;

///
/// Create a new ParticleEmitter.
///
/// \param pg ParticleGroup
/// \param num_particles Number of particles to be managed in emitter
/// \param x Position x for ParticleEmitter
/// \param y Position y for ParticleEmitter
/// \return Newly created ParticleEmitter
///
extern ParticleEmitter* ParticleEmitter_new(ParticleGroup* pg, int num_particles, int x, int y);

///
/// Initialize ParticleEmitter
///
/// \param emitter ParticleEmitter to initialize
/// \param pg ParticleGroup
/// \param num_particles Number of particles to be managed in emitter
/// \param x Position x for ParticleEmitter
/// \param y Position y for ParticleEmitter
/// \return True if initialize successfully, otherwise return false.
///
extern bool ParticleEmitter_init(ParticleEmitter* emitter, ParticleGroup* pg, int num_particles, int x, int y);

///
/// Update particles managed by ParticleEmitter
///
/// \param emitter ParticleEmitter to update
/// \param delta_time Elapsed time since last frame
///
extern void ParticleEmitter_update(ParticleEmitter* emitter, float delta_time);

///
/// Apply force to all particles.
///
/// \param emitter ParticleEmitter
/// \param force_x Force to apply in x direction
/// \param force_y Force to apply in y direction
///
extern void ParticleEmitter_apply_force(ParticleEmitter* emitter, int force_x, int force_y);

///
/// Render particles managed by ParticleEmitter
///
/// \param emitter ParticleEmitter to render
///
extern void ParticleEmitter_render(ParticleEmitter* emitter);

///
/// Free ParticleEmitter's internals
///
/// \param emitter ParticleEmitter's internals to be freed
///
extern void ParticleEmitter_free_internals(ParticleEmitter* emitter);

///
/// Free ParticleEmitter
///
/// \param emitter ParticleEmitter to be freed
///
extern void ParticleEmitter_free(ParticleEmitter* emitter);

#endif
