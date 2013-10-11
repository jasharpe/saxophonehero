#include "easing.hpp"

// this should be a function that is 0 at 0.
double easing_function(double t) {
  return t * t;
}

double standard_easing(double t) {
  return (1 - t) * easing_function(t) + t * (-easing_function(1 - t) + 1);
}
