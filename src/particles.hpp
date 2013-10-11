#ifndef _PARTICLES__H
#define _PARTICLES__H

#include "time.hpp"
#include "algebra.hpp"
#include <iostream>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include "pass.hpp"

enum physics_t {
  NOTE_BOARD,
  SAX_END,
  SAX_END2,
  SMOKE
};

struct Particle {
  Particle() : colour(0, 0, 0) {
    creation_time = 0;
    death_time = 0;
  }

  // the time the particle was created
  unsigned long creation_time;
  // how long the particle has left to live, in milliseconds
  unsigned long death_time;

  // note the two following are in whatever coordinates you want, the
  // particle merely keeps track of them
  // for you.

  // the current position of the particle
  Point3D position;
  // the current velocity of the particle
  Vector3D velocity;

  Colour colour;
};

class Particles {
public:
  Particles(physics_t physics);

  void create_particle(int TTL, Point3D position, Vector3D velocity, Colour colour);
  void draw(pass_t pass, bool reflected, unsigned long time, Matrix4x4 rotation_matrix = Matrix4x4());

  physics_t m_physics;
  std::vector<Particle> m_particles;
};

#endif
