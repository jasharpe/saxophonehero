#include "particles.hpp"
#include <cmath>
#include "debug.hpp"

const int PARTICLES = 50;

// TODO: this should have some physics
// parameters. For now assume gravity is
// in the y direction. (So a < 0 for a regular
// kind of gravity.)
Particles::Particles(physics_t physics) : m_physics(physics) {
  for (int i = 0; i < PARTICLES; i++) {
    // push on an already dead particle
    m_particles.push_back(Particle());
  }
}

void Particles::create_particle(int TTL, Point3D position, Vector3D velocity, Colour colour) {
  // finds an unused particle and uses it
  // for this new particle
  unsigned long creation_time = get_time();
  unsigned long death_time = creation_time + TTL;
  Particle* dead_particle = NULL;
  for (unsigned int i = 0; i < m_particles.size(); i++) {
    Particle* particle = &m_particles[i];
    if (particle->death_time < creation_time) {
      dead_particle = particle;
      break;
    }
  }
  if (dead_particle == NULL) {
    std::cerr << "No more particles available" << std::endl;
    return;
  }
  dead_particle->creation_time = creation_time;
  dead_particle->death_time = death_time;
  dead_particle->position = position;
  dead_particle->velocity = velocity;
  dead_particle->colour = colour;
}

const double SIZE = 0.05;
const int STAR_POLYS = 1;
const int STAR_PTS[STAR_POLYS] = { 10 };
const int NOTE_POLYS = 3;
const int NOTE_PTS[NOTE_POLYS] = { 20, 4, 4 };
const int BAD_NOTE_POLYS = 1;
const int BAD_NOTE_PTS[BAD_NOTE_POLYS] = { 20 };
const int SMOKE_POLYS = 1;
const int SMOKE_PTS[SMOKE_POLYS] = { 20 };
const double RPS = -1;
// wobbles per second for notes
const double WPS = 2;
void Particles::draw(pass_t pass, bool reflected, unsigned long time, Matrix4x4 rotation_matrix) {
  // the points of a star in CCW order
  static Point3D star[STAR_POLYS][20];
  // the point involved in a note in CCW order
  static Point3D note[NOTE_POLYS][20];
  static Point3D bad_note[BAD_NOTE_POLYS][20];
  static Point3D smoke_particle[SMOKE_POLYS][20];
  static bool init = false;
  if (!init) {
    // initialize star
    for (int i = 0; i < STAR_PTS[0]; i++) {
      double angle = (2 * M_PI * i) / STAR_PTS[0];
      double scale = 1.5;
      if (i % 2 == 1) {
        scale *= 0.4;
      }
      star[0][i] = Point3D(scale * std::sin(angle), scale * std::cos(angle), 0);
    }

    double note_scale = 1.5;
    double note_x_scale = 1.35;
    for (int i = 0; i < NOTE_PTS[0]; i++) {
      double angle = (2 * M_PI * i) / NOTE_PTS[0];
      note[0][i] = Point3D(note_x_scale * note_scale * std::sin(angle), note_scale * std::cos(angle), 0);
    }
    double shaft_width = 0.25 * note_scale;
    double shaft_height = 3.5 * note_scale;
    double shaft_offset = note_scale * note_x_scale;
    note[1][3] = Point3D(shaft_offset, 0, 0);
    note[1][2] = Point3D(shaft_offset, 3.5 * note_scale, 0);
    note[1][1] = Point3D(shaft_offset - shaft_width, 3.5 * note_scale, 0);
    note[1][0] = Point3D(shaft_offset - shaft_width, 0, 0);
    double offshoot_height = 0.4 * note_scale;
    double offshoot_width = 1.6 * note_scale;
    double offshoot_down = 1.5;
    note[2][3] = Point3D(shaft_offset, shaft_height, 0);
    note[2][2] = Point3D(shaft_offset, shaft_height - offshoot_height, 0);
    note[2][1] = Point3D(shaft_offset + offshoot_width, shaft_height - offshoot_height - offshoot_down, 0);
    note[2][0] = Point3D(shaft_offset + offshoot_width, shaft_height - offshoot_down, 0);

    for (int i = 0; i < BAD_NOTE_PTS[0]; i++) {
      double angle = (2 * M_PI * i) / BAD_NOTE_PTS[0];
      double scale = 4;
      if (i % 2 == 1) {
        scale *= 0.6;
      }
    bad_note[0][i] = Point3D(1.2 * scale * std::sin(angle), scale * std::cos(angle), 0);
    }

    for (int i = 0; i < SMOKE_PTS[0]; i++) {
      double angle = (2 * M_PI * i) / SMOKE_PTS[0];
      smoke_particle[0][i] = Point3D(std::sin(angle), std::cos(angle), 0);
    }

    /*smoke_particle[0][0] = Point3D(-1, -1, 0);
    smoke_particle[0][1] = Point3D(-1, 1, 0);
    smoke_particle[0][2] = Point3D(1, 1, 0);
    smoke_particle[0][3] = Point3D(1, -1, 0);*/

    init = true;
  }

  glPushMatrix();
  if (reflected) {
    glScaled(-1, 1, 1);
  }
  for (unsigned int i = 0; i < m_particles.size(); i++) {
    Particle* p = &m_particles[i];
    // check that particle is still alive before drawing it.
    if (time < p->death_time && time > p->creation_time) {
      glPushMatrix();
      int lifespan = p->death_time - p->creation_time;
      int elapsed = time - p->creation_time;
      double t = elapsed / double(lifespan);
      
      double dt = 10 * elapsed / 1000.0;
      Vector3D v = 0.05 * p->velocity;
        
      Point3D position;
      if (m_physics == NOTE_BOARD) {
        Vector3D a = Vector3D(0, -0.05, 0);
        position = Point3D(
          p->position[0] + dt * v[0] + 0.5 * a[0] * dt * dt,
          p->position[1] + dt * v[1] + 0.5 * a[1] * dt * dt,
          p->position[2] + dt * v[2] + 0.5 * a[2] * dt * dt
        );
      } else if (m_physics == SAX_END || m_physics == SAX_END2) {
        Vector3D a = -0.1 * v;
        double first_phase = 0.8;
        if (m_physics == SAX_END2) {
          first_phase = 0.0;
        }
        double second_dt = 0;
        if (dt > first_phase) {
          second_dt = dt - first_phase;
        }

        Vector3D a2;
        if (m_physics == SAX_END) {          
          a2 = Vector3D(0, 0.02, 0);
        } else {
          a2 = Vector3D(0, -0.04, 0);
        }
        position = Point3D(
          p->position[0] + dt * v[0] + 0.5 * a[0] * dt * dt + 0.5 * a2[0] * second_dt * second_dt,
          p->position[1] + dt * v[1] + 0.5 * a[1] * dt * dt + 0.5 * a2[1] * second_dt * second_dt,
          p->position[2] + dt * v[2] + 0.5 * a[2] * dt * dt + 0.5 * a2[2] * second_dt * second_dt
        );
      } else if (m_physics == SMOKE) {
        // constant speed
        position = Point3D(
          p->position[0] + dt * v[0],
          p->position[1] + dt * v[1],
          p->position[2] + dt * v[2]
        );
      }
      glTranslated(position[0], position[1], position[2]);

      // rotate the particle so it always faces the camera
      if (m_physics == SAX_END || m_physics == SAX_END2 || m_physics == SMOKE) {
        double vals[16];
        if (false && reflected) {
          rotation_matrix.dump(vals);
        } else {
          rotation_matrix.invert().dump(vals);
        }
        glMultMatrixd(vals);
      }

      // fade the particle out quadratically
      double alpha = 1 - t * t;
      if (pass == NORMAL) {
        glColor4d(p->colour.R(), p->colour.G(), p->colour.B(), alpha);
      } else {
        glColor4d(0, 0, 0, alpha);
      }
      
      if (m_physics == NOTE_BOARD) {
        // rotate the particle at RPS rotations per second
        double rotation = 360 * elapsed / double(1000.0 * RPS);
        if (v[0] < 0 || (v[0] == 0 && v[2] > 0)) {
          rotation = -rotation;
        }
        glRotated(rotation, 0, 0, 1);
      } else if (m_physics == SAX_END) {
        // the note should bobble slightly
        double wobble = 20;
        double rotation = (1 - t) * wobble * sin(0.5 + 2 * M_PI * WPS * elapsed / double(1000.0));
        glRotated(rotation, 0, 0, 1);
      }

      // find position
      double size;
      if (m_physics == NOTE_BOARD) {
        size = SIZE;
      } else if (m_physics == SMOKE) {
        size = 0.025;
      } else {
        size = SIZE;
      }
      glScaled(size, size, size);
      int* points_by_poly = NULL;
      int polys = 0;
      if (m_physics == NOTE_BOARD) {
        polys = STAR_POLYS;
        points_by_poly = (int*) STAR_PTS;
      } else if (m_physics == SAX_END) {
        polys = NOTE_POLYS;
        points_by_poly = (int*) NOTE_PTS;
      } else if (m_physics == SAX_END2) {
        polys = BAD_NOTE_POLYS;
        points_by_poly = (int*) BAD_NOTE_PTS;
      } else if (m_physics == SMOKE) {
        polys = SMOKE_POLYS;
        points_by_poly = (int*) SMOKE_PTS;
      }
      // draw the object in CCW order
      for (int poly = 0; poly < polys; poly++) {
        Point3D* object = NULL;
        if (m_physics == NOTE_BOARD) {
          object = star[poly];
        } else if (m_physics == SAX_END) {
          object = note[poly];
        } else if (m_physics == SAX_END2) {
          object = bad_note[poly];
        } else if (m_physics == SMOKE) {
          object = smoke_particle[poly];
        }

        int points = points_by_poly[poly];
        if (pass == NORMAL) {
          debug("begin");
          glBegin(GL_POLYGON);
          for (int j = points - 1; j >= 0; j--) {
            Point3D vertex = object[j];
            glVertex3d(vertex[0], vertex[1], vertex[2]);
          }
          glEnd();
        } else if (pass == OUTLINE) {
          glBegin(GL_POLYGON);
          // this is a slightly weird order, but it's oddly non-functional
          // if I do 0 to 9, so I do 1 to 9 and then 0.
          for (int j = 1; j < points + 1; j++) {
            Point3D vertex = object[j % points];
            if (m_physics == NOTE_BOARD) {
              glVertex3d(vertex[0], vertex[1], vertex[2] + 0.001);
            } else {
              glVertex3d(vertex[0], vertex[1], vertex[2]);
            }
          }
          debug("end");
          glEnd();
        }
      }
      glPopMatrix();
    }
  }
  glPopMatrix();
}
