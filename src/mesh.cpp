#include "mesh.hpp"
#include <iostream>
#include <math.h>
#include "checkerror.hpp"
#include "debug.hpp"

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : Primitive(),
    m_verts(verts),
    m_faces(faces)
{
  std::vector<std::vector<int> > vertex_to_face;
  for (unsigned int i = 0; i < m_verts.size(); i++) {
    std::vector<int> faces;
    vertex_to_face.push_back(faces);
  }
  for (unsigned int i = 0; i < m_faces.size(); i++) {
    Face* face = &m_faces[i];
    for (unsigned int j = 0; j < face->size(); j++) {
      (&vertex_to_face[face->at(j)])->push_back(i);
    }
  }
  // debug
  /*for (unsigned int i = 0; i < vertex_to_face.size(); i++) {
    std::vector<int>* faces = &vertex_to_face[i];
    for (unsigned int j = 0; j < faces->size(); j++) {
      std::cerr << faces->at(j) << " ";
    }
    std::cerr << std::endl;
  }*/

  // compute the normals to each vertex as the average of the normals of
  // each face.
  for (unsigned int i = 0; i < m_verts.size(); i++) {
    //std::cerr << (i / (double) m_verts.size()) << std::endl;
    // a vector containing the face normals of all adjacent faces
    std::vector<Vector3D> face_normals;
    std::vector<int>* faces = &vertex_to_face[i];
    for (unsigned int j = 0; j < faces->size(); j++) {
      face_normals.push_back(face_normal(m_faces[faces->at(j)]));
    }
    Vector3D normal(0, 0, 0);
    for (unsigned int j = 0; j < face_normals.size(); j++) {
      normal = normal + face_normals[j];
    }
    normal.normalize();
    m_normals.push_back(normal);
  }
}

Vector3D Mesh::face_normal(Face& face) {
  // find the normal to the face
  Vector3D first_edge = m_verts[face[1]] - m_verts[face[0]];
  first_edge.normalize();
  Vector3D second_edge = m_verts[face[2]] - m_verts[face[1]];
  second_edge.normalize();
  Vector3D normal = first_edge.cross(second_edge);
  normal.normalize();
  return normal;
}

void Mesh::prelim() {}

void Mesh::draw(Point3D light_position, GLuint texture, int texture_setting, pass_t pass, bool print_position) {
  (void) print_position;
  double tmpMatrixData[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, tmpMatrixData);
  Matrix4x4 trans(tmpMatrixData, true);
  Matrix4x4 invtrans = trans.invert().transpose();
  
  check_error(9812);

  glActiveTexture(GL_TEXTURE2);
  if (texture != 0) {
    glEnable(GL_TEXTURE_2D);
    check_error(928378);
    glBindTexture(GL_TEXTURE_2D, texture);
    check_error(52232);
  } else {
    glDisable(GL_TEXTURE_2D);
    check_error(11242);
  }
  glActiveTexture(GL_TEXTURE1);
  
  check_error(8334);

  for (std::vector<Mesh::Face>::const_iterator it = m_faces.begin(); it != m_faces.end(); it++) {
    Face face = *it;

    glBegin(GL_POLYGON);
    debug("begin");
    check_error(9882);
    for (unsigned int i = 0; i < face.size(); i++) {      
      Vector3D normal = invtrans * m_normals[face[i]];
      normal.normalize();
      Point3D vert = m_verts[face[i]];
      Point3D trans_vert = trans * vert;

      if (texture != 0) {
        if (texture_setting == 0 || texture_setting == 2) {
          int scale = 4;
          if (texture_setting == 2) {
            scale = 2;
          }
          //std::cerr << "normal: " << normal << std::endl;
          if (std::abs(normal[2]) == 1) {
            glMultiTexCoord2d(GL_TEXTURE2, trans_vert[0] / scale, trans_vert[1] / scale);
          } else if (std::abs(normal[1]) == 1) {
            glMultiTexCoord2d(GL_TEXTURE2, trans_vert[0] / scale, trans_vert[2] / scale);
          } else if (std::abs(normal[0]) == 1) {
            glMultiTexCoord2d(GL_TEXTURE2, trans_vert[2] / scale, trans_vert[1] / scale);
          } else {
            glMultiTexCoord2d(GL_TEXTURE2, 0, 0);
          }
        } else {
          if (i == 0) {
            glMultiTexCoord2d(GL_TEXTURE2, 0, 0);
          } else if (i == 1) {
            glMultiTexCoord2d(GL_TEXTURE2, 0, 1);
          } else if (i == 2) {
            glMultiTexCoord2d(GL_TEXTURE2, 1, 1);
          } else {
            glMultiTexCoord2d(GL_TEXTURE2, 1, 0);
          }
        }
      }
      if (pass != NORMAL) {
        glTexCoord1d(0);
        glVertex3d(vert[0], vert[1], vert[2]);
      } else {
        // lighting
        Vector3D light_angle = (light_position - trans_vert);
        light_angle.normalize();
        double tmpShade = std::max(0.0, normal.dot(light_angle));
        glTexCoord1d(tmpShade);

        glVertex3d(vert[0], vert[1], vert[2]);
      }
    }
    debug("end");
    glEnd();
  }

  glActiveTexture(GL_TEXTURE2);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE1);
  check_error(8372);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
