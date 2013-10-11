#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include "algebra.hpp"
#include "pass.hpp"

void get_display_list(int& i);

class Primitive {
public:
  Primitive();
  virtual ~Primitive();
  void walk_gl(Point3D light_position, bool picking, GLuint texture, int texture_setting, pass_t pass, bool print_position);
  void redraw();
  virtual void prelim() = 0;
  virtual void draw(Point3D light_position, GLuint texture, int texture_setting, pass_t pass, bool print_position) = 0;

  // set this to true in order to cause the primitive to regenerate
  // its display list.
  bool m_redraw;
  // the number of the display list corresponding to this primitive.
  GLuint m_display_list;
  // this display list is for outlines,
  // shaded and shadow drawings - should
  // never have to be redrawn.
  GLuint m_simple_display_list;
  int m_how_many;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  virtual void prelim();
  virtual void draw(Point3D light_position, GLuint texture, int texture_setting, pass_t pass, bool print_position);
};

#endif
