#ifndef CS488_QUATERNION_HPP
#define CS488_QUATERNION_HPP

#include "algebra.hpp"

typedef Vector4D Quat;

Quat rot_to_quat(Matrix4x4 rot);
Matrix4x4 quat_to_rot(Quat quat);

#endif
