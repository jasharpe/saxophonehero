#ifndef _CAMERA__H
#define _CAMERA__H

#include "algebra.hpp"
#include "quaternion.hpp"
#include <vector>

enum Easing { LINEAR, QUADRATIC };

struct Animation {
  Animation(unsigned long start_time, unsigned long end_time, Quat initial_rotation, Quat final_rotation, Vector3D initial_position, Vector3D final_position)
    : start_time(start_time), end_time(end_time), initial_rotation(initial_rotation), final_rotation(final_rotation), initial_position(initial_position), final_position(final_position)  {
  }

  long start_time;
  long end_time;
  Quat initial_rotation;
  Quat final_rotation;
  Vector3D initial_position;
  Vector3D final_position;
};

inline std::ostream& operator <<(std::ostream& os, const Animation& a)
{
  return os << "start time: " << a.start_time << ", end time: " << a.end_time << ", initial rotation: " << a.initial_rotation << ", final rotation: " << a.final_rotation << ", initial position: " << a.initial_position << ", final position: " << a.final_position;
}

class Camera {
public:
  Camera();

  void load_animations();
  Animation* get_animation(double time);

  long m_length;
  std::vector<Animation> m_animations;
};

#endif
