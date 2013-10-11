#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

void check_error(int unique) {
  int err = glGetError();
  while (err != 0) {
    std::cerr << "Unique: " << unique << ", Error: 0x" << std::hex << err << std::dec << std::endl;
    err = glGetError();
    sleep(1);
    exit(1);
  }
}
