#include <GL/gl.h>
#include <GL/glu.h>

GLuint shader_texture[1];
float shader_data[32][3];
void initialize_cel_shading() {
  glDisable(GL_LIGHTING);

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &shader_texture[0]);
  glBindTexture(GL_TEXTURE_1D, shader_texture[0]);

  glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  for (int i = 0; i < 32; i++) {
    float val = 1.0;
    if (i < 14) val = 0.5;
    if (i < 4) val = 0.35;
    // slightly brighter values
    //if (i < 14) val = 0.7;
    //if (i < 4) val = 0.5;
    shader_data[i][0] = shader_data[i][1] = shader_data[i][2] = val;
  }

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 32, 0, GL_RGB, GL_FLOAT, shader_data);
}
