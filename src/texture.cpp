#include "texture.hpp"
#include "bitmap.hpp"

GLuint load_texture(char* texture_bmp) {
  Bitmap *image;
  image = new Bitmap();
  if (image == NULL) {
    return 0;
  }

  GLuint texture[1];
  if (image->loadBMP(texture_bmp)) {
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    glActiveTexture(GL_TEXTURE1);
    //std::cerr << "Loaded texture into " << texture[0] << std::endl;
  } else {
    std::cerr << "Failed to load texture: " << image->error << std::endl;
    texture[0] = 0;
  }

  delete image;
  return texture[0];
}
