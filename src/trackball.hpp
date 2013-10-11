#ifndef TRACKBALL_HPP
#define TRACKBALL_HPP

#include "algebra.hpp"   /* For the definition of Matrix4x4 */

/* Function prototypes */
Vector3D calcRotVec(double newX, double newY,
                 double oldX, double oldY,
                 double diameter);
Matrix4x4 axisRotMatrix(double x, double y, double z);

#endif
