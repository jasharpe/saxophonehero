#include "primitive.hpp"
#include <iostream>
#include <cmath>
#include "checkerror.hpp"

void get_display_list(GLuint& i) {
  i = glGenLists(1);
  if (i == 0) {
    std::cerr << "Failed to get a display list" << std::endl;
  }
}

Primitive::Primitive() {
  m_redraw = true;
  m_display_list = 0;
  m_simple_display_list = 0;
  m_how_many = 0;
}

Primitive::~Primitive()
{
}

void Primitive::redraw() {
  m_redraw = true;
}

void Primitive::walk_gl(Point3D light_position, bool picking, GLuint texture, int texture_setting, pass_t pass, bool print_position) {
  (void) picking;
  glPushMatrix();
  prelim();
  if (pass != NORMAL) {
    if (m_simple_display_list == 0) {
      if (m_simple_display_list == 0) {
        get_display_list(m_simple_display_list);
      }
      glNewList(m_simple_display_list, GL_COMPILE);
      draw(Point3D(0, 0, 0), texture, texture_setting, OUTLINE, false);
      check_error(13513);
      glEndList();
    }
    check_error(7382);
    glCallList(m_simple_display_list);
    check_error(3772);
  } else {
    if (m_display_list == 0) {
      get_display_list(m_display_list);
    }
    if (m_redraw && m_how_many == 0) {
      glNewList(m_display_list, GL_COMPILE);
      draw(light_position, texture, texture_setting, pass, print_position && light_position[0] == 0);
      check_error(12882);
      glEndList();
      m_redraw = false;
    }
    m_how_many = (m_how_many + 1) % 5;
    check_error(29247);
    glCallList(m_display_list);
    check_error(3992);
  }
  check_error(92883);
  glPopMatrix();
  check_error(14326);
}

Sphere::Sphere() : Primitive() {}

Sphere::~Sphere()
{
}

// the number of degrees between each vertex
const int SPACE = 15;
const int VERTEX_COUNT = (90 / SPACE) * (360 / SPACE) * 8;
Point3D VERTICES[VERTEX_COUNT];
bool SPHERE_CREATED = false;
void create_sphere() {
  // n is the current vertex
  int n = 0;
  // a and b are the polar coordinates of the vertex
  double space_r = SPACE / 180.0 * M_PI;
  for (double b = 0; b <= 90 - SPACE; b += SPACE) {
    double b_r = b / 180.0 * M_PI;
    for (double a = 0; a <= 360 - SPACE; a += SPACE) {
      double a_r = a / 180.0 * M_PI;
      VERTICES[n][0] = sin(a_r) * sin(b_r);
      VERTICES[n][1] = cos(a_r) * sin(b_r);
      VERTICES[n][2] = cos(b_r);
      n++;
      
      VERTICES[n][0] = sin(a_r) * sin(b_r + space_r);
      VERTICES[n][1] = cos(a_r) * sin(b_r + space_r);
      VERTICES[n][2] = cos(b_r + space_r);
      n++;

      VERTICES[n][0] = sin(a_r + space_r) * sin(b_r + space_r);
      VERTICES[n][1] = cos(a_r + space_r) * sin(b_r + space_r);
      VERTICES[n][2] = cos(b_r + space_r);
      n++;

      VERTICES[n][0] = sin(a_r + space_r) * sin(b_r);
      VERTICES[n][1] = cos(a_r + space_r) * sin(b_r);
      VERTICES[n][2] = cos(b_r);
      n++;
    }
  }

  for (int m = 0; m < VERTEX_COUNT / 2; m++) {
    VERTICES[VERTEX_COUNT - m - 1] = VERTICES[m];
    VERTICES[VERTEX_COUNT - m - 1][2] *= -1;
  }

  SPHERE_CREATED = true;
}

void Sphere::prelim() {
  if (!SPHERE_CREATED) {
    create_sphere();
  }
  // rotate spheres so that their ugly holes point up and down
  glRotated(90, 1, 0, 0);
}

void Sphere::draw(Point3D light_position, GLuint texture, int texture_setting, pass_t pass, bool print_position) {
  // if I had more time I could try texturing spheres too
  (void) texture;
  (void) texture_setting;
  Matrix4x4 transformationMatrix;
  Matrix4x4 invtrans;
  if (pass == NORMAL) {  
    double tmpMatrixData[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, tmpMatrixData);
    transformationMatrix = Matrix4x4(tmpMatrixData, true);
    invtrans = transformationMatrix.invert().transpose();
  }
 
  static double max_y = -1000;
  static double min_y = 1000;
  static double max_z = -1000;
  static double min_z = 1000;
  if (print_position) {
    Point3D position = transformationMatrix * Point3D(0, 0, 0);
    max_y = std::max(max_y, position[1]);
    min_y = std::min(min_y, position[1]);
    max_z = std::max(max_z, position[2]);
    min_z = std::min(min_z, position[2]);
    std::cerr << "Position: " << position << std::endl;
    //std::cerr << "Light Position: " << light_position << std::endl;
    std::cerr << "max_y: " << max_y << ", min_y: " << min_y << ", max_z: " << max_z << ", min_z: " << min_z << std::endl;
  }

  glBegin(GL_QUADS);
  // draw backwards so that clockwiseness is right way around
  for (int n = VERTEX_COUNT - 1; n >= 0; n--) {
    Point3D tmpVertex = VERTICES[n];
    
    if (pass != NORMAL) {
      glTexCoord1d(0);
    } else {
      // we're on a sphere, so the normal is just the vertex point
      Vector3D tmpNormal =
        invtrans * Vector3D(tmpVertex[0], tmpVertex[1], tmpVertex[2]);
      tmpNormal.normalize();

      Vector3D light_angle = (light_position - transformationMatrix * tmpVertex);
      light_angle.normalize();
      
      double tmpShade = std::max(0.0, tmpNormal.dot(light_angle));
      glTexCoord1d(tmpShade);
    }
    // this is what we would do normally, but we're cel shading so it's
    // different
    //glNormal3d(VERTICES[n][0], VERTICES[n][1], VERTICES[n][2]);
    glVertex3d(tmpVertex[0], tmpVertex[1], tmpVertex[2]);
  }
  glEnd();
}
