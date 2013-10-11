#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include "algebra.hpp"
#include "pass.hpp"

void toggle_textures();

class Material {
public:
  Material(const char* texture_bmp);
  virtual ~Material();
  virtual void apply_gl(pass_t pass, bool picked) = 0;

  void init();

  GLuint texture();

  int m_setting;

protected:
  Material()
  {
  }

  char* m_texture_bmp;
  GLuint m_texture;

private:
  bool m_initialized;
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const char* texture_bmp, int setting);
  virtual ~PhongMaterial();

  virtual void apply_gl(pass_t pass, bool picked);

private:
  // use m_kd for ambient colour too
  Colour m_kd; // diffuse
  Colour m_ks; // specular

  double m_shininess;
};


#endif
