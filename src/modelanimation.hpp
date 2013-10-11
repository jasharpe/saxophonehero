#ifndef _MODEL_ANIMATION__H
#define _MODEL_ANIMATION__H

// this can be applied to any node and is capable of translations
// only.
class ModelAnimation {
public:
  ModelAnimation(long start_time, long end_time, Vector3D initial_position, Vector3D final_position) : start_time(start_time), end_time(end_time), initial_position(initial_position), final_position(final_position) {
  }

  long start_time;
  long end_time;
  Vector3D initial_position;
  Vector3D final_position;
};

// this can be applied to joints and is capable of rotations
class JointAnimation {
public:
  JointAnimation(long start_time, long end_time, double initial_x, double final_x, double initial_y, double final_y) : start_time(start_time), end_time(end_time), initial_x(initial_x), final_x(final_x), initial_y(initial_y), final_y(final_y) {
  }

  long start_time;
  long end_time;
  double initial_x;
  double final_x;
  double initial_y;
  double final_y;
};

#endif
