#ifndef Particle_h_
#define Particle_h_

#include "ParticleGroup.h"
#include <stdbool.h>

///
/// Particle handles individual particle.
///
typedef struct {
  /// mass
  int mass;

  /// pos x
  int x;
  /// pos y
  int y;

  // velocity x
  float velx;
  // velocity y
  float vely;

  // acceleration x
  float accx;
  // acceleration y
  float accy;

  /// current frame rendering this particle
  int frame;

  /// remaining lifetime of particle, in seconds
  float lifetime;
  /// lifetime initially set
  float original_lifetime;

  /// scale of particle. Use for both x & y axis.
  float scale;

  /// whether this particle is dead
  bool is_dead;

  /// (internally used) aniimation time count used for external to keep track of time to render current frame of animation. It's in seconds.
  float anim_timecount;
} Particle;

/// Create a new particle
///
/// \param x Position x for this particle
/// \param y Position y for this particle
/// \return Newly created a new particle on heap, otherwise return NULL if failed.
///
extern Particle* Particle_new(int x, int y);

/// Initialize particle
/// User still needs to configure other attributes of Particle.
/// Just directly modify its attributes of struct.
///
/// \param p Particle to initialize its values
/// \param x Position x for this particle
/// \param y Positino y for this particle
/// \return True if initialize successfully, otherwise return false.
///
extern bool Particle_init(Particle* p, int x, int y);

/// Reset state of particle.
///
/// \param p Particle to be reseted.
///
extern void Particle_reset(Particle* p);

/// Free internals of Particle.
/// Use this to free memory created by Paraticle_init().
///
extern void Particle_free_internals(Particle* p);

/// Free memory of Particle.
/// Use this to free memory created by Particle_new()
///
extern void Particle_free(Particle* p);

#endif
