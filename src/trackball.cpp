#include <math.h>
#include "trackball.hpp"

/*******************************************************
 * 
 * void vCalcRotVec(float fNewX, float fNewY, 
 *                  float fOldX, float fOldY,
 *                  float fDiameter,
 *                  float *fVecX, float *fVecY, float *fVecZ);
 *
 *    Calculates a rotation vector based on mouse motion over
 *    a virtual trackball.
 *
 *    The fNew and fOld mouse positions
 *    should be in 'trackball' space. That is, they have been
 *    transformed into a coordinate system centered at the middle
 *    of the trackball. fNew, fOld, and fDiameter must all be specified
 *    in the same units (pixels for example).
 *
 * Parameters: fNewX, fNewY - New mouse position in trackball space.
 *                            This is the second point along direction
 *                            of rotation.
 *             fOldX, fOldY - Old mouse position in trackball space.
 *                            This is the first point along direction
 *                            of rotation.
 *             fDiameter - Diameter of the trackball. This should
 *                         be specified in the same units as fNew and fOld.
 *                         (ie, usually pixels if fNew and fOld are transformed
 *                         mouse positions)
 *             fVec - The output rotation vector. The length of the vector
 *                    is proportional to the angle of rotation.
 *
 *******************************************************/
Vector3D calcRotVec(double newX, double newY, double oldX, double oldY,
    double diameter) {
  double newVecX, newVecY, newVecZ,        /* Vector corresponding to new mouse location */
         oldVecX, oldVecY, oldVecZ,        /* Vector corresponding to old mouse location */
         length;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   newVecX = newX * 2.0 / diameter;
   newVecY = newY * 2.0 / diameter;
   newVecZ = (1.0 - newVecX * newVecX - newVecY * newVecY);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (newVecZ < 0.0) {
      length = sqrt(1.0 - newVecZ);
      newVecZ  = 0.0;
      newVecX /= length;
      newVecY /= length;
   } else {
      newVecZ = sqrt(newVecZ);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   oldVecX    = oldX * 2.0 / diameter;
   oldVecY    = oldY * 2.0 / diameter;
   oldVecZ    = (1.0 - oldVecX * oldVecX - oldVecY * oldVecY);
 
   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (oldVecZ < 0.0) {
      length = sqrt(1.0 - oldVecZ);
      oldVecZ  = 0.0;
      oldVecX /= length;
      oldVecY /= length;
   } else {
      oldVecZ = sqrt(oldVecZ);
   }

   /* Generate rotation vector by calculating cross product:
    * 
    * oldVec x newVec.
    * 
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   return Vector3D(
       oldVecY * newVecZ - newVecY * oldVecZ,
       oldVecZ * newVecX - newVecZ * oldVecX,
       oldVecX * newVecY - newVecX * oldVecY);
}

/*******************************************************
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat)
 *    
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *    
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
Matrix4x4 axisRotMatrix(double x, double y, double z) {
    float radians, invLength, newX, newY, newZ;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    radians = sqrt(x * x + y * y + z * z);

    /* If the vector has zero length - return the identity matrix */
    if (radians > -0.000001 && radians < 0.000001) {
      return Matrix4x4();
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    invLength = 1 / radians;
    newX = x * invLength;
    newY = y * invLength;
    newZ = z * invLength;

    /* Create the arbitrary axis rotation matrix */
    double sinAlpha = sin(radians);
    double cosAlpha = cos(radians);
    double t = 1 - cosAlpha;

    Matrix4x4 retMat;

    retMat[0][0] = cosAlpha + newX * newX * t;
    retMat[0][1] = newX * newY * t + newZ * sinAlpha;
    retMat[0][2] = newX * newZ * t - newY * sinAlpha;
    retMat[0][3] = 0;

    retMat[1][0] = newX * newY * t - sinAlpha * newZ;
    retMat[1][1] = cosAlpha + newY * newY * t;
    retMat[1][2] = newY * newZ * t + sinAlpha * newX;
    retMat[1][3] = 0;

    retMat[2][0] = newZ * newX * t + sinAlpha * newY;
    retMat[2][1] = newZ * newY * t - sinAlpha * newX;
    retMat[2][2] = cosAlpha + newZ * newZ * t;
    retMat[2][3] = 0;

    retMat[3][0] = 0;
    retMat[3][1] = 0;
    retMat[3][2] = 0;
    retMat[3][3] = 1;

    return retMat;
}

