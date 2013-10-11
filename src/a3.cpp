#include <cmath>
#include "a3.hpp"

const double PI = 3.14159265359;

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis)
{
  angle = angle * (PI / 180);
  if (axis == 'x') {
    double vals[] = {
      1, 0, 0, 0,
      0, cos(angle), -sin(angle), 0,
      0, sin(angle), cos(angle), 0,
      0, 0, 0, 1
    };
    return Matrix4x4(vals);
  } else if (axis == 'y') {
    double vals[] = {
      cos(angle), 0, sin(angle), 0,
      0, 1, 0, 0,
      -sin(angle), 0, cos(angle), 0,
      0, 0, 0, 1
    };
    return Matrix4x4(vals);
  } else if (axis == 'z') {
    double vals[] = {
      cos(angle), -sin(angle), 0, 0,
      sin(angle), cos(angle), 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    };
    return Matrix4x4(vals);
  } else {
    // incorrect input
    std::cerr << "Non-existent axis supplied to rotation(double, char): "
      << axis << std::endl;
    return Matrix4x4();
  }
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement)
{
  double vals[] = {
    1, 0, 0, displacement[0],
    0, 1, 0, displacement[1],
    0, 0, 1, displacement[2],
    0, 0, 0, 1
  };
  return Matrix4x4(vals);
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale)
{
  double vals[] = {
    scale[0], 0, 0, 0,
    0, scale[1], 0, 0,
    0, 0, scale[2], 0,
    0, 0, 0, 1
  };
  return Matrix4x4(vals);
}

Matrix4x4 projection(double fov, double aspect, double near, double far) {
  // angle in radians
  double angle = fov * (PI / 180);
  int plus_minus = 1;
  double vals[] = {
    1 / (tan(angle / 2) * aspect), 0, 0, 0,
    0, 1 / tan(angle / 2), 0, 0,
    0, 0, plus_minus * (far + near) / (far - near), (-2 * far * near) / (far - near),
    0, 0, plus_minus * 1, 0
  };
  return Matrix4x4(vals);
}

Vector4D matrixTimesVector4D(const Matrix4x4& M, const Vector4D& v)
{
  return Vector4D(
    v[0] * M[0][0] + v[1] * M[0][1] + v[2] * M[0][2] + v[3] * M[0][3],
    v[0] * M[1][0] + v[1] * M[1][1] + v[2] * M[1][2] + v[3] * M[1][3],
    v[0] * M[2][0] + v[1] * M[2][1] + v[2] * M[2][2] + v[3] * M[2][3],
    v[0] * M[3][0] + v[1] * M[3][1] + v[2] * M[3][2] + v[3] * M[3][3]);
}
