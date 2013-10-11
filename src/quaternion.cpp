// algorithms/some code obtained from http://www.j3d.org/matrix_faq/matrfaq_latest.html

#include <cmath>
#include "quaternion.hpp"

Quat rot_to_quat(Matrix4x4 rot) {
  double mat[16];
  rot.dump(mat);

  // calculate the trace
  double T = 1 + mat[0] + mat[5] + mat[10];
  double X, Y, Z, W, S;
  if (T > 0.000001) {
    // trace greater than 0
    S = std::sqrt(T) * 2;
    X = ( mat[9] - mat[6] ) / S;
    Y = ( mat[2] - mat[8] ) / S;
    Z = ( mat[4] - mat[1] ) / S;
    W = 0.25 * S;
  } else if ( mat[0] > mat[5] && mat[0] > mat[10] )  {	// Column 0: 
    S  = sqrt( 1.0 + mat[0] - mat[5] - mat[10] ) * 2;
    X = 0.25 * S;
    Y = (mat[4] + mat[1] ) / S;
    Z = (mat[2] + mat[8] ) / S;
    W = (mat[9] - mat[6] ) / S;
  } else if ( mat[5] > mat[10] ) { // Column 1: 
    S  = sqrt( 1.0 + mat[5] - mat[0] - mat[10] ) * 2;
    X = (mat[4] + mat[1] ) / S;
    Y = 0.25 * S;
    Z = (mat[9] + mat[6] ) / S;
    W = (mat[2] - mat[8] ) / S;
  } else { // Column 2:
    S  = sqrt( 1.0 + mat[10] - mat[0] - mat[5] ) * 2;
    X = (mat[2] + mat[8] ) / S;
    Y = (mat[9] + mat[6] ) / S;
    Z = 0.25 * S;
    W = (mat[4] - mat[1] ) / S;
  }

  return Quat(X, Y, Z, W);
}

Matrix4x4 quat_to_rot(Quat quat) {
  double mat[16];
  double X = quat[0];
  double Y = quat[1];
  double Z = quat[2];
  double W = quat[3];

  double xx = X * X;
  double xy = X * Y;
  double xz = X * Z;
  double xw = X * W;
  double yy = Y * Y;
  double yz = Y * Z;
  double yw = Y * W;
  double zz = Z * Z;
  double zw = Z * W;
  mat[0]  = 1 - 2 * ( yy + zz );
  mat[1]  =     2 * ( xy - zw );
  mat[2]  =     2 * ( xz + yw );
  mat[4]  =     2 * ( xy + zw );
  mat[5]  = 1 - 2 * ( xx + zz );
  mat[6]  =     2 * ( yz - xw );
  mat[8]  =     2 * ( xz - yw );
  mat[9]  =     2 * ( yz + xw );
  mat[10] = 1 - 2 * ( xx + yy );
  mat[3]  = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
  mat[15] = 1;
  return Matrix4x4(mat, true);
}
