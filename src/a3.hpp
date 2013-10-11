#ifndef CS488_A3_HPP
#define CS488_A3_HPP

#include "algebra.hpp"

// You should implement these functions, and use them from viewer.cpp

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis);

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement);

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale);

// Return a matrix to represent a projection with the
// given parameters. Does not normalize.
Matrix4x4 projection(double fov, double aspect, double near, double far);

Vector4D matrixTimesVector4D(const Matrix4x4& M, const Vector4D& v);

#endif
