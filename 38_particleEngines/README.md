# Changes from orginal

* Add full fledge of particle engine that is reusable for future taking into account for velocity, acceleration, alpha and scaling. It also supports playing animation.
* Not use Dot to be source of particles, but use `ParticleEmitter` itself.
* All particle textures are packed into one texture for performance, and then render in clipped way in SDL.
* Particle system supports force application in both direction x, y.
* Particle has mass.
