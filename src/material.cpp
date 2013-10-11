#include "material.hpp"
#include "texture.hpp"

bool textures_enabled = false;

void toggle_textures() {
  textures_enabled = !textures_enabled;
}

GLuint Material::texture() {
  if (textures_enabled) {
    return m_texture;
  } else {
    return 0;
  }
}

Material::Material(const char* texture_bmp) {
  m_setting = 0;
  m_texture = 0;
  m_texture_bmp = NULL;
  if (texture_bmp != NULL) {
    int size = strlen(texture_bmp) + 1;
    m_texture_bmp = new char[size];
    strcpy(m_texture_bmp, texture_bmp);
  }
  m_initialized = false;
}

Material::~Material()
{
}

void Material::init() {
  if (m_initialized) {
    return;
  }

  if (m_texture_bmp != NULL) {
    m_texture = load_texture(m_texture_bmp);
  }

  m_initialized = true;
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, const char* texture_bmp, int setting)
  : Material(texture_bmp), m_kd(kd), m_ks(ks), m_shininess(shininess)
{
  m_setting = setting;
}

PhongMaterial::~PhongMaterial()
{
}

// returns a texture ID
void PhongMaterial::apply_gl(pass_t pass, bool picked)
{
  (void) pass;
  if (!picked) {
    glColor3d(m_kd.R(), m_kd.G(), m_kd.B());
  } else {
    glColor3d(1.0, 1.0, 0.0);
  }
  /*glColorMaterial(GL_FRONT, GL_SPECULAR);
  if (picked) {
    float diffuse[4] = {1.0, 1.0, 0.0, 0.5};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
  } else {
    float diffuse[4] = {m_kd.R(), m_kd.G(), m_kd.B(), 0.1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
  }
  glColor3d(m_ks.R(), m_ks.G(), m_ks.B());
  float emission[4] = {0, 0, 0, 1};
  glMaterialfv(GL_FRONT, GL_EMISSION, emission);
  glMaterialf(GL_FRONT, GL_SHININESS, m_shininess);*/
}
