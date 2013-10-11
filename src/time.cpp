#include "time.hpp"
#include <sys/timeb.h>
#include <iostream>

unsigned long speed_top = 4;
const unsigned long speed_bottom = 4;

void change_speed(int speed_delta) {
  if (speed_top == 0 && speed_delta < 0) return;
  speed_top += speed_delta;
  if (speed_top > 40) {
    speed_top = 40;
  }
}

int get_speed() {
  return (int) speed_top;
}

void reset_speed() {
  speed_top = 4;
}

// returns time in milliseconds
unsigned long get_time() {
  struct timeb tp;
  ftime(&tp);
  return ((unsigned long) tp.millitm + 1000 * (unsigned long) tp.time) * speed_top / speed_bottom;
}

unsigned long true_get_time() {
  struct timeb tp;
  ftime(&tp);
  return (unsigned long) tp.millitm + 1000 * (unsigned long) tp.time;
}
