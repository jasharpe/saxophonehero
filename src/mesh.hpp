#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  virtual void prelim();
  virtual void draw(Point3D light_position, GLuint texture, int texture_setting, pass_t pass, bool print_position);

  typedef std::vector<int> Face;
  
private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  // indexed the same way as m_verts
  std::vector<Vector3D> m_normals;

  Vector3D face_normal(Face& face);

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
