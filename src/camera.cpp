#include "camera.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

Camera::Camera() {
  load_animations();
}

Animation* Camera::get_animation(double time) {
  // look for the first animation with start_time after time, and return that.
  for (unsigned int i = 0; i < m_animations.size(); i++) {
    Animation* animation = &m_animations[i];
    if (animation->start_time <= time && animation->end_time >= time) {
      return animation;
    }
  }
  return NULL;
}

void Camera::load_animations() {
  m_animations = std::vector<Animation>();
  
  const char* file = "data/giantsteps.camera";
  std::ifstream animations(file);
  if (animations.is_open()) {
    std::string line;
    std::getline(animations, line);
    m_length = atol(line.c_str());
    while (animations) {
      if (!std::getline(animations, line)) {
        break;
      }

      // ignore comments and blank lines, ie a line that starts
      // with a space ;).
      if (line[0] == '#' || line[0] == ' ') {
        continue;
      }

      std::istringstream line_stream(line);
      std::string element;
      double X, Y, Z, W;

      getline(line_stream, element, ',');
      long start_time = atol(element.c_str());
      getline(line_stream, element, ',');
      long end_time = atol(element.c_str());

      // get initial_rotation
      getline(line_stream, element, ',');
      X = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Y = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Z = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      W = strtod(element.c_str(), NULL);
      Quat initial_rotation(X, Y, Z, W);

      // get initial_position
      getline(line_stream, element, ',');
      X = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Y = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Z = strtod(element.c_str(), NULL);
      Vector3D initial_position(X, Y, Z);

      // get final_rotation
      getline(line_stream, element, ',');
      X = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Y = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Z = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      W = strtod(element.c_str(), NULL);
      Quat final_rotation(X, Y, Z, W);

      // get final_position
      getline(line_stream, element, ',');
      X = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Y = strtod(element.c_str(), NULL);
      getline(line_stream, element, ',');
      Z = strtod(element.c_str(), NULL);
      Vector3D final_position(X, Y, Z);

      m_animations.push_back(Animation(start_time, end_time, initial_rotation, final_rotation, initial_position, final_position));
    }
  } else {
    std::cerr << "Unable to open file " << file << std::endl;
  }
}
