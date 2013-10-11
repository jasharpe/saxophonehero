#include "viewer.hpp"
#include "algebra.hpp"
#include <iostream>
#include <math.h>
#include "soundmanager.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "trackball.hpp"
#include "checkerror.hpp"
#include "celshading.hpp"
#include "time.hpp"
#include "texture.hpp"
#include "a3.hpp"
#include "quaternion.hpp"
#include "easing.hpp"
#include "debug.hpp"
#include "material.hpp"

const int GAME_CLOCK_MILLIS = 16;

UndoItem::UndoItem(int id, double x, double y) :
  m_id(id), m_x(x), m_y(y) {
}

UndoItem::~UndoItem() {
}

void Viewer::load(SceneNode* root) {
  m_picked_nodes = std::set<int>();
  m_in_clicked_mode[0] = m_in_clicked_mode[1] = m_in_clicked_mode[2] = false;
  m_root = root;
  m_first = true;
  reset_all();
}

Viewer::Viewer(SceneNode* root) :
  m_rolling_fps(0),
  m_note_particles(NOTE_BOARD),
  m_failed_undo(false),
  m_something_changed(false),
  m_undo_items(),
  m_undo_stack_pointer(-1),
  //m_light_position(-10, 15, 20),
  //m_light_position(-10, 20, -10),
  m_light_position(0, 10, -10),
  m_root(root),
  m_picked_nodes(),
  m_translation(0, 0, 0),
  m_mode(MODE_POSITION),
  m_circle(false),
  m_zbuffer(false),
  m_backface_cull(false),
  m_frontface_cull(false),
  m_fps(false),
  m_camera_script(false),
  m_draw_board(false),
  m_compatibility(false),
  m_reflections(false),
  m_cel_shading(false),
  m_outlines(false),
  m_textures(false),
  m_first(true),
  m_shadows(false),
  m_show_light_depth(false),
  m_light_fov(100.0),
  m_light_near(0.6),
  m_light_far(145.0),
  // for some reason (floating point precision), has to be just
  // under 0.5 to not create hideous artifacts.
  m_bias(0.499),
  m_direction_viewed(0)
{
  //m_rotation = rotation(180, 'y') * m_rotation;
  //m_translation[0] += 7;
  m_light_look_at[0] = Point3D(0, 0, -20);
  m_light_look_at[1] = Point3D(0, 0, -10);
  m_light_look_at[2] = Point3D(0, 0, 0);
  m_light_look_at[3] = Point3D(10, 0, -10);
  m_light_look_at[4] = Point3D(-10, 0, -10);
  m_buttons[0] = m_buttons[1] = m_buttons[2] = false;

  m_note = import_lua("data/notes.lua");
  m_top = import_lua("data/top.lua");

  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Ask for an OpenGL Setup with
  //  - red, green and blue component colour
  //  - a depth buffer to avoid things overlapping wrongly
  //  - double-buffered rendering to avoid tearing/flickering
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
  if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);

  //GLuint shader = glCreateShader(GL_VERTEX_SHADER);

  // Register the fact that we want to receive these events
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::VISIBILITY_NOTIFY_MASK);
}

Viewer::~Viewer()
{
  // Nothing to do here right now.
}

void Viewer::invalidate()
{
  // Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}

const int SHADOW_MAP_SIZE = 512;
const int directions = 5;
GLuint m_shadow_map_texture[directions];
void Viewer::initialize_shadow_map() {
  // set up 2D texture stuff
  glActiveTexture(GL_TEXTURE1);
  glGenTextures(directions, m_shadow_map_texture);
}

void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
  if (!gldrawable)
    return;

  if (!gldrawable->gl_begin(get_gl_context()))
    return;
  
  glClearColor( 0.4, 0.4, 0.4, 0.0 );
  glEnable(GL_DEPTH_TEST);

  gldrawable->gl_end();

  // initialize static resources
  initialize_shadow_map();
  initialize_cel_shading();

  // set up game timer
  sigc::slot0<bool> my_slot = sigc::mem_fun(*this, &Viewer::on_game_clock_timeout);

  // This is where we connect the slot to the Glib::signal_timeout()
  sigc::connection conn = Glib::signal_timeout().connect(my_slot,
          GAME_CLOCK_MILLIS);
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
  (void)event;
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  long start = true_get_time();
  long time = (get_time() - m_game.m_start_time) % m_game.m_length;
  draw(time);
  //std::cerr << "Frame took: " << elapsed << std::endl;
  double a = 0.05;
  long true_elapsed = true_get_time() - start;
  double fps;
  if (true_elapsed > 0) {
    fps = 1000.0 / true_elapsed;
  } else {
    fps = 2000.0;
  }
  m_rolling_fps = (1 - a) * m_rolling_fps + a * fps;
  //std::cerr << m_rolling_fps << std::endl;
  if (m_fps) {
    draw_fps(m_rolling_fps);
  }
  draw_score(m_game.m_score);
  glFlush();

  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

  // Reset to modelview matrix mode
  
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

void Viewer::do_model_transformation() {
  // do model translation
  double vals[16];
  m_rotation.dump(vals);
  glMultMatrixd(vals);
  glTranslated(m_translation[0], m_translation[1], m_translation[2]);
  // do model rotation:
  // translate to origin, rotate, then translate back
  //Vector3D root_translation = m_root->get_translation();
  //glTranslated(root_translation[0], root_translation[1], root_translation[2]);
  
  //glTranslated(-root_translation[0], -root_translation[1], -root_translation[2]);
}

double light_proj_matrix[16];
double light_view_matrix[directions][16];
void Viewer::draw(long time) {
  check_error(1245);

  // Clear framebuffer
  if (m_failed_undo) {
    glClearColor(1.0, 0.4, 0.4, 0.0);
  } else {
    glClearColor(0.4, 0.4, 0.4, 0.0);
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_NORMALIZE);
  
  if (m_shadows) {
    // draw only back faces in the shadow map
    glCullFace(GL_FRONT);
    // disable color writes since we just care about depth buffer
    glColorMask(0, 0, 0, 0);
    
    // set up projection matrix (same for all directions)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(m_light_fov, 1.0, m_light_near, m_light_far);
    glGetDoublev(GL_PROJECTION_MATRIX, light_proj_matrix);
    
    check_error(12234);
    for (int direction = 0; direction < directions; direction++) {
      // PASS 1 - Create shadow map
      glActiveTexture(GL_TEXTURE1);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_shadow_map_texture[direction]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      // enable shadow comparison
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
      glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
      glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_SIZE,
        SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
        NULL);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();

      check_error(12351);

      gluLookAt(m_light_position[0], m_light_position[1], m_light_position[2], m_light_look_at[direction][0], m_light_look_at[direction][1], m_light_look_at[direction][2], 1, 0, 0);

      // one pixel in the viewport for each pixel in the shadow map
      glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

      check_error(12352);

      // draw scene for shadow map
      glGetDoublev(GL_MODELVIEW_MATRIX, light_view_matrix[direction]);
      check_error(9273);
      actually_draw(Point3D(0, 0, 0), time, OUTLINE);

      check_error(7134);

      glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
        SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    
      glClear(GL_DEPTH_BUFFER_BIT);
      glPopMatrix();
    }
    check_error(5132);
    glCullFace(GL_BACK);
    glColorMask(1, 1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    // Draw three layers - shaded, lit, and outlines
    glClear(GL_DEPTH_BUFFER_BIT);

    if (m_show_light_depth) {
      glColor3d(1, 1, 1);
      glActiveTexture(GL_TEXTURE1);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

      glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      gluOrtho2D(-1, 1, -1, 1);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glLoadIdentity();
      glActiveTexture(GL_TEXTURE1);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_shadow_map_texture[m_direction_viewed]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
      debug("begin");
      glBegin(GL_QUADS);
      glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
      glVertex3f( -1.0f, -1.0f, 0.0f );

      glMultiTexCoord2f(GL_TEXTURE1, 0, 1);
      glVertex3f( -1.0f, 1.0f, 0.0f );

      glMultiTexCoord2f(GL_TEXTURE1, 1, 1);
      glVertex3f( 1.0f, 1.0f, 0.0f );

      glMultiTexCoord2f(GL_TEXTURE1, 1, 0);
      glVertex3f( 1.0f, -1.0f, 0.0f );
      debug("end");
      glEnd();
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      return;
    }
  }

  // set up random settings for shaded and lit passes
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_1D);
  // this allows the bright areas to overwrite the
  // shadowed areas.
  glDepthFunc(GL_LEQUAL);

  if (m_zbuffer) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
  } else {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
  }

  if (m_backface_cull || m_frontface_cull) {
    glEnable(GL_CULL_FACE);
    if (m_backface_cull && m_frontface_cull) {
      glCullFace(GL_FRONT_AND_BACK);
    } else if (m_backface_cull) {
      glCullFace(GL_FRONT);
    } else {
      glCullFace(GL_BACK);
    }
  } else {
    glDisable(GL_CULL_FACE);
  }

  // Set up for perspective drawing 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, get_width(), get_height());
  gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  do_model_transformation();

  // draw it all for reflections
  {
    glPushMatrix();
    glTranslated(20, 0, 0);
    glScaled(-1.0, 1.0, 1.0);
    if (m_reflections) {
      draw_inner(time, true);
    }
    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(0.7, 0.7, 0.7, 0.4);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPushMatrix();
    glTranslated(0, -4.5 / 2, 0);
    debug("begin");
    glBegin(GL_QUADS);
    glVertex3d(10, 0, -30);
    glVertex3d(10, 0, 10);
    glVertex3d(10, 10, 10);
    glVertex3d(10, 10, -30);
    debug("end");
    glEnd();
    glPopMatrix();
    glDisable(GL_BLEND);
  }

  // set up random settings for shaded and lit passes
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_1D);
  // this allows the bright areas to overwrite the
  // shadowed areas.
  glDepthFunc(GL_LEQUAL);

  if (m_zbuffer) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
  } else {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
  }

  if (m_backface_cull || m_frontface_cull) {
    glEnable(GL_CULL_FACE);
    if (m_backface_cull && m_frontface_cull) {
      glCullFace(GL_FRONT_AND_BACK);
    } else if (m_backface_cull) {
      glCullFace(GL_BACK);
    } else {
      glCullFace(GL_FRONT);
    }
  } else {
    glDisable(GL_CULL_FACE);
  }

  draw_inner(time, false);

  if (m_draw_board) {
    // clear depth buffer so that following stuff is drawn on top
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw playing board
    draw_board(NORMAL);

    // draw playing board outline
    setup_outline(false);
    if (m_outlines) {
      draw_board(OUTLINE);
    }
    teardown_outline(false);
  }

  if (m_circle) {
    draw_trackball_circle();
  }

  glFlush();

  check_error(24542);
}

void Viewer::draw_inner(long time, bool reflected) {
  glPushMatrix();
  
  // get light position after view has been transformed
  double light_matrix_data[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, light_matrix_data);
  Matrix4x4 light_matrix(light_matrix_data, true);
  Point3D adj_light_position = light_matrix * m_light_position;

  glPolygonMode(GL_FRONT, GL_FILL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*if (reflected) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
  }*/
  if (m_shadows) {
    // PASS 2 - draw with shaded light intensity
    check_error(55135);
    actually_draw(adj_light_position, time, SHADED);
    check_error(9282);

    // PASS 3 - draw lit regions with dynamic intensity
    double bias_vals[16] =
      { m_bias, 0, 0, m_bias,
        0, m_bias, 0, m_bias,
        0, 0, m_bias, m_bias,
        0, 0, 0, 1 };
    Matrix4x4 bias_matrix(bias_vals);
    for (int direction = 0; direction < directions; direction++) {
      Matrix4x4 texture_matrix = bias_matrix * Matrix4x4(light_proj_matrix, true) * Matrix4x4(light_view_matrix[direction], true);
     
      glActiveTexture(GL_TEXTURE1);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_shadow_map_texture[direction]);

      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      double* row_s = texture_matrix[0];
      double eye_plane_s[] =
        { row_s[0], row_s[1], row_s[2], row_s[3] };
      glTexGendv(GL_S, GL_EYE_PLANE, eye_plane_s);
      glEnable(GL_TEXTURE_GEN_S);
      
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      double* row_t = texture_matrix[1];
      double eye_plane_t[] =
        { row_t[0], row_t[1], row_t[2], row_t[3] };
      glTexGendv(GL_T, GL_EYE_PLANE, eye_plane_t);
      glEnable(GL_TEXTURE_GEN_T);
      
      glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      double* row_r = texture_matrix[2];
      double eye_plane_r[] =
        { row_r[0], row_r[1], row_r[2], row_r[3] };
      glTexGendv(GL_R, GL_EYE_PLANE, eye_plane_r);
      glEnable(GL_TEXTURE_GEN_R);
      
      glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
      double* row_q = texture_matrix[3];
      double eye_plane_q[] =
        { row_q[0], row_q[1], row_q[2], row_q[3] };
      glTexGendv(GL_Q, GL_EYE_PLANE, eye_plane_q);
      glEnable(GL_TEXTURE_GEN_Q);      

      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GEQUAL, 0.99);

      check_error(72364);
      if (m_cel_shading) {
        actually_draw(adj_light_position, time, NORMAL);
      }
      check_error(12273);
    }
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glDisable(GL_ALPHA_TEST);
  } else {
    if (m_cel_shading) {
      actually_draw(adj_light_position, time, NORMAL);
    } else {
      actually_draw(adj_light_position, time, SHADED);
    }
  }

  glActiveTexture(GL_TEXTURE2);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  unsigned long particle_time = get_time();
  draw_particles(NORMAL, reflected, particle_time);
  glEnable(GL_CULL_FACE);

  // render the model again, this time for cel shaded outlines
  setup_outline(reflected);
  check_error(38074);
  if (m_outlines) {
    actually_draw(adj_light_position, time, OUTLINE);
  }
  teardown_outline(reflected);
  setup_outline(false);
  if (m_outlines) {
    draw_particles(OUTLINE, reflected, particle_time);
  }
  check_error(1772);
  teardown_outline(false);

  glPopMatrix();
}

void Viewer::setup_outline(bool reflected) {
  glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
  glColor4d(0, 0, 0, 1);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glColor4d(0, 0, 0, 1);
  if (reflected) {
    glPolygonMode(GL_FRONT, GL_LINE);
  } else {
    glPolygonMode(GL_BACK, GL_LINE);
  }
  glLineWidth(4.0);
  glEnable(GL_CULL_FACE);
  if (reflected) {
    glCullFace(GL_BACK);
  } else {
    glCullFace(GL_FRONT);
  }
  glDepthFunc(GL_LEQUAL);
}

void Viewer::teardown_outline(bool reflected) {
  glDepthFunc(GL_LEQUAL);
  glPolygonMode(GL_BACK, GL_FILL);
  if (reflected) {
    glCullFace(GL_FRONT);
  } else {
    glCullFace(GL_BACK);
  }
  glDisable(GL_CULL_FACE);
  glLineWidth(1.0);
}

void Viewer::play_music() {
  SM.PlayMusic(0);
}

void Viewer::actually_draw(Point3D light_position, long time, pass_t pass) {
  m_root->walk_gl(light_position, time, m_picked_nodes, pass, false);
}

void Viewer::draw_particles(pass_t pass, bool reflected, unsigned long time) {
  if (pass == NORMAL) {
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_1D);
  }
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_BLEND);
  if (pass == OUTLINE) {
    glLineWidth(2.0);
  }
  m_root->draw_particles(pass, reflected, time, m_rotation);
  if (pass == OUTLINE) {
    glLineWidth(4.0);
  }
  glDisable(GL_BLEND);
  if (pass == NORMAL) {
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_1D);
  }
}

void Viewer::draw_board(pass_t pass) {
  glActiveTexture(GL_TEXTURE0);
  glDisable(GL_TEXTURE_1D);
  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE2);
  glDisable(GL_TEXTURE_2D);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, get_width(), get_height());
  gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(0, -0.92, -5);
  glRotated(-90, 1, 0, 0);

  static GLuint texture = 0;
  if (texture == 0) {
    texture = load_texture((char*) "data/board2.bmp");
  }

  glActiveTexture(GL_TEXTURE2);
  if (texture != 0 && m_textures) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
  }

  double unit_x = 0.75;
  double unit_y = 2.5;
  double z = 0;
  double z2 = 0.002;

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_BLEND);

  debug("begin");
  if (pass == NORMAL) {
    glBegin(GL_QUADS);
  } else {
    glBegin(GL_LINES);
  }
  // period of board in milliseconds
  long period = 1500;
  // offset should be periodic from 0 to 1 linearly in period period
  double offset = ((get_time() % period) / double(period));
  //offset -= int(offset);
  int pieces = 10;
  double alpha_start = 0.7;
  for (int piece = 0; piece < pieces; piece++) {
    double alpha_top = 1;
    double alpha_bottom = 1;
    if (piece >= pieces * alpha_start) {
      alpha_top = 1 - ((piece + 1) - pieces * alpha_start) / (pieces - pieces * alpha_start);
      alpha_bottom = 1 - (piece - pieces * alpha_start) / (pieces - pieces * alpha_start);
    }
    double piece_top_y = -unit_y + 2 * unit_y * ((piece + 1) / double(pieces));
    double piece_bottom_y = -unit_y + 2 * unit_y * (piece / double(pieces));
    double top_tex_y = offset + (piece + 1) / double(pieces);
    //top_tex_y -= int(top_tex_y);
    double bottom_tex_y = offset + piece / double(pieces);
    //bottom_tex_y -= int(bottom_tex_y);
    if (pass == NORMAL) {
      glColor4d(1, 1, 1, alpha_top);
      glMultiTexCoord2d(GL_TEXTURE2, 1, top_tex_y);
      glVertex3d(-unit_x, piece_top_y, z);
      glColor4d(1, 1, 1, alpha_bottom);
      glMultiTexCoord2d(GL_TEXTURE2, 1, bottom_tex_y);
      glVertex3d(-unit_x, piece_bottom_y, z);
      glColor4d(1, 1, 1, alpha_bottom);
      glMultiTexCoord2d(GL_TEXTURE2, 0, bottom_tex_y);
      glVertex3d(unit_x, piece_bottom_y, z);
      glColor4d(1, 1, 1, alpha_top);
      glMultiTexCoord2d(GL_TEXTURE2, 0, top_tex_y);
      glVertex3d(unit_x, piece_top_y, z);
    } else if (pass == OUTLINE) {
      glColor4d(0, 0, 0, alpha_top);
      glVertex3d(unit_x, piece_top_y, z2);
      glColor4d(0, 0, 0, alpha_bottom);
      glVertex3d(unit_x, piece_bottom_y, z2);
      glColor4d(0, 0, 0, alpha_bottom);
      glVertex3d(-unit_x, piece_bottom_y, z2);
      glColor4d(0, 0, 0, alpha_top);
      glVertex3d(-unit_x, piece_top_y, z2);
    }
  }
  debug("end");
  glEnd();

  glDisable(GL_TEXTURE_2D);
    
  // draw notes
  std::vector<Note>* notes = &m_game.m_notes;
  // how long has elapsed since the start of the most recent repetition.
  //std::cerr << "length: " << m_game.m_length << std::endl;
  unsigned long cur_time = get_time();
  long elapsed = (cur_time - m_game.m_start_time) % m_game.m_length;
  //std::cerr << "elapsed: " << elapsed << std::endl;
  // how far into the past and into the future to render notes
  // in milliseconds.
  double in_the_future_ratio = 0.9;
  long in_the_past = long((1 - in_the_future_ratio) * double(period));
  long in_the_future = long(in_the_future_ratio * double(period));
  long in_the_future_beginning = 0;
  if (m_game.m_length - elapsed < period) {
    in_the_future_beginning = period - (m_game.m_length - elapsed);
  }
  //std::cerr << "in the future beginning: " << in_the_future_beginning << std::endl;
  double base_y = unit_y - 2 * unit_y * in_the_future_ratio;
  double z3 = 0.01;
  
  glPushMatrix();
  glTranslated(0, base_y, 0);
  glRotated(90, 1, 0, 0);
  glScaled(1 / (2 * unit_x), 1 / (2 * unit_x), 1 / (2 * unit_x));

  if (pass == OUTLINE) {
    glLineWidth(2.0);
  }
  m_note_particles.draw(pass, false, cur_time);
  if (pass == OUTLINE) {
    glLineWidth(4.0);
  }

  // TODO: draw little circle pads instead of this line
  if (pass == NORMAL) {
    glColor4d(0.3, 0.3, 0.3, 1);
    debug("begin");
    glBegin(GL_LINES);
    glVertex3d(-1 + 0.001, z3, 0);
    glVertex3d(1 - 0.001, z3, 0);
    debug("end");
    glEnd();
  }
  glPopMatrix();

  for (unsigned int i = 0; i < notes->size(); i++) {
    Note* note = &notes->at(i);
    // oversimplified version
    bool draw_note = false;
    double x = note->note * (2 * unit_x / 6.0) - unit_x;
    double y;
    if ((note->time > elapsed && note->time < elapsed + in_the_future)
      || (note->time < in_the_future_beginning)) {
      double time = (note->time - elapsed);
      // if the note is after the end of the song
      if (time < 0) {
        time += m_game.m_length;
      }
      y = in_the_future_ratio * (2 * unit_y) * time / double(in_the_future) + base_y;
      draw_note = true;
    } else if ((note->time <= elapsed && note->time > elapsed - in_the_past - 100)) {
      double time = (elapsed - note->time);
      y = base_y - (1 - in_the_future_ratio) * (2 * unit_y) * time / double(in_the_past);
      draw_note = true;
    }
    // draw note if necessary
    if (draw_note) {
      // fade in notes at the top of the note board
      double alpha = 1;
      if (note->hit) {
        // number of seconds over which to fade out
        // the note.
        const double FADEOUT = 100;
        long time_since_hit = cur_time - note->hit;
        alpha = 1 - time_since_hit / FADEOUT;
        if (alpha < 0) {
          continue;
        }
      } else {
        double alpha_test = (y + unit_y) / (2 * unit_y);
        if (alpha_test > alpha_start) {
          alpha = 1 - (alpha_test - alpha_start) / (1 - alpha_start);
        }
      }
      
      if (pass == NORMAL) {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_1D);
      }

      glPushMatrix();
      glTranslated(x, y, 0);
      double scale = 0.125;
      glScaled(scale, scale, scale);
      if (pass == NORMAL) {
        glColor4d(1, 1, 1, alpha);
      } else {
        glColor4d(0, 0, 0, alpha);
      }
      m_top->walk_gl(Point3D(5, 5, 0), elapsed, m_picked_nodes, pass, false);
      
      if (pass == NORMAL) {
        switch (note->note) {
          case 1:
            glColor4d(0, 1, 0, alpha);
            break;
          case 2:
            glColor4d(1, 0, 0, alpha);
            break;
          case 3:
            glColor4d(1, 1, 0, alpha);
            break;
          case 4:
            glColor4d(0, 0, 1, alpha);
            break;
          case 5:
            glColor4d(1, 0.5, 0.25, alpha);
            break;
        }
      } else {
        glColor4d(0, 0, 0, alpha);
      }
      glRotated(90, 1, 0, 0);
      if (pass == NORMAL && !m_cel_shading) {
        m_note->walk_gl(Point3D(5, 5, 0), elapsed, m_picked_nodes, SHADED, false);
      } else {
        m_note->walk_gl(Point3D(5, 5, 0), elapsed, m_picked_nodes, pass, false);
      }
      glPopMatrix();
    }
  }
}

// the size of the buffer to use for hits
const int BUFF_SIZE = 64;

/**
 * Find the object under (x, y) on the screen
 */
bool Viewer::select(int x, int y) {

  // do picking
  // initialize buffer to 0
  GLuint buff[BUFF_SIZE] = {0};
  GLint view[4];

  // initialize picking stuff
  glSelectBuffer(BUFF_SIZE, buff);
  glGetIntegerv(GL_VIEWPORT, view);
  glRenderMode(GL_SELECT);

  long elapsed = (get_time() - m_game.m_start_time) % m_game.m_length;

  // set up projection and draw
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(x, view[3] - y, 1.0, 1.0, view);
    gluPerspective(40.0, (view[2] - view[0]) / float(view[3] - view[1]), 0.1, 1000.0);
    // draw
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      glLoadIdentity();
      glInitNames();
      glPushName(-1);
      do_model_transformation();
      actually_draw(Point3D(0, 0, 0), elapsed, SHADED);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glFlush();
  glMatrixMode(GL_MODELVIEW);

  GLint hits = glRenderMode(GL_RENDER);
  bool something_picked = false;
  
  GLuint* buff_ptr = (GLuint *) buff;
  GLuint minZ = 0xffffffff;
  GLuint min_z_num_names = 0;
  GLuint* names_ptr = NULL;
  for (int i = 0; i < hits; i++) {
    GLuint num_names = buff_ptr[0];
    bool hit = true;
    
    for (unsigned int j = 0; j < num_names; j++) {
      if (buff_ptr[j + 3] == (unsigned int) -1) {
        // skipping
        hit = false;
      }
    }
    
    if (hit) {
      if (buff_ptr[1] < minZ) {
        something_picked = true;
        min_z_num_names = num_names;
        minZ = buff_ptr[1];
        names_ptr = buff_ptr + 3;
      }
    }

    buff_ptr += num_names + 3;
  }

  if (something_picked) {
    for (unsigned int i = 0; i < min_z_num_names; i++) {
      if (!set_contains2(m_picked_nodes, names_ptr[i])) {
        m_picked_nodes.insert(names_ptr[i]);
      } else {
        m_picked_nodes.erase(names_ptr[i]);
      }
    }
  }

  return something_picked;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
  m_buttons[event->button - 1] = true;
  m_button_init_x = event->x;
  m_button_init_y = event->y;
  m_in_clicked_mode[event->button - 1] = true;

  switch (m_mode) {
    case MODE_POSITION:
      switch (event->button) {
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
      }
      break;
    case MODE_JOINTS:
      switch (event->button) {
        case 1:
          if (select(event->x, event->y)) {
            invalidate();
          }
          break;
        case 2:
        case 3:
          if (m_undo_items.size() == 0) {
            add_undo_stack_entry();
          }
          break;
      }
      break;
    case MODE_LIGHT:
      switch (event->button) {
        case 1:
          m_direction_viewed = (m_direction_viewed + 1) % directions;
          break;
        case 2:
        case 3:
          break;
      }
      invalidate();
      break;
  }
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  m_buttons[event->button - 1] = false;

  switch (m_mode) {
    case MODE_POSITION:
      switch (event->button) {
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
      }
      break;
    case MODE_JOINTS:
      switch (event->button) {
        case 1:
          break;
        case 2:
        case 3:
          if (m_something_changed) {
            // clear all stack entries above m_undo_stack_pointer
            m_undo_items.erase(m_undo_items.begin() + m_undo_stack_pointer + 2, m_undo_items.end());
            add_undo_stack_entry();
            m_undo_stack_pointer++;
            // only put another entry on if something else happens
            m_something_changed = false;
          }
          break;
      }
      break;
  }

  return true;
}

void Viewer::add_undo_stack_entry() {
  // record the rotations of all joints into an undo stack
  // entry
  std::vector<JointNode*>* joints = get_all_joints();
  std::vector<UndoItem> positions;
  for (std::vector<JointNode*>::iterator it = joints->begin(); it != joints->end(); it++) {
    JointNode* joint = *it;
    positions.push_back(UndoItem(joint->m_id, joint->m_current_x, joint->m_current_y));
  }
  m_undo_items.push_back(positions);
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
  int delta_x = m_button_init_x - event->x;
  int delta_y = m_button_init_y - event->y;
  
  switch (m_mode) {
    case MODE_POSITION:
      for (int button = 1; button <= 3; button++) {
        if (m_buttons[button - 1] && m_in_clicked_mode[button - 1]) {
          Vector3D x_translation;
          Vector3D y_translation;
          switch (button) {
            case 1:
              x_translation = m_rotation.invert() * Vector3D(1, 0, 0);
              x_translation.normalize();
              y_translation = m_rotation.invert() * Vector3D(0, -1, 0);
              y_translation.normalize();
              m_translation = m_translation +
                -0.06 * delta_x * x_translation +
                -0.06 * delta_y * y_translation;
              break;
            case 2:
              y_translation = m_rotation.invert() * Vector3D(0, 0, 1);
              y_translation.normalize();
              m_translation = m_translation +
                0.06 * delta_y * y_translation;
              break;
            case 3:
              double diameter = get_trackball_radius() * 2;
              Vector3D rotation_vector =
                calcRotVec(event->x - get_width() / 2, event->y - get_height() / 2,
                    m_button_init_x - get_width() / 2, m_button_init_y - get_height() / 2,
                    diameter);
              // some sign hacks to make it work right, possibly
              // didn't port the code properly.
              Matrix4x4 rotation_matrix = axisRotMatrix(
                    rotation_vector[0],
                    -rotation_vector[1],
                    rotation_vector[2]);
              m_rotation = rotation_matrix * m_rotation;
              break;
          }
        }
      }
      invalidate();
      break;
    case MODE_JOINTS:
      for (int button = 1; button <= 3; button++) {
        if (m_buttons[button - 1] && m_in_clicked_mode[button - 1]) {
          switch (button) {
            case 1:
              break;
            case 2:
              // rotate all selected joints along x axis!
              for (std::set<int>::const_iterator it = m_picked_nodes.begin(); it != m_picked_nodes.end(); it++) {
                // get the joint corresponding to the selected gizmo
                SceneNode* joint = get_node(*it)->m_parent;
                double angle = joint->rotate('x', -delta_y);
                if (angle != 0) {
                  m_something_changed = true;
                }
              }
              invalidate();
              break;
            case 3:
              // rotate all selected joints!
              for (std::set<int>::const_iterator it = m_picked_nodes.begin(); it != m_picked_nodes.end(); it++) {
                // get the joint corresponding to the selected gizmo
                SceneNode* joint = get_node(*it)->m_parent;
                double angle = joint->rotate('y', -delta_x);
                if (angle != 0) {
                  m_something_changed = true;
                }
              }
              invalidate();
              break;
          }
        }
      }
      break;
    case MODE_SHADOW:
      for (int button = 1; button <= 3; button++) {
        if (m_buttons[button - 1] && m_in_clicked_mode[button - 1]) {
          switch (button) {
            case 1:
              // adjust fov
              m_light_fov += 2.5 * delta_x;
              m_light_fov = std::min(180.0, std::max(0.0, m_light_fov));
              std::cerr << "new fov: " << m_light_fov << std::endl;
              break;
            case 2:
              m_light_near += 0.5 * delta_x;
              m_light_near = std::max(0.1, m_light_near);
              m_light_far += 5.0 * delta_y;
              std::cerr << "near: " << m_light_near << ", far: " << m_light_far << std::endl;
              break;
            case 3:
              m_bias += 0.001 * delta_x;
              std::cerr << "bias: " << m_bias << std::endl;
              break;
          }
        }
      }
      invalidate();
      break;
  }

  m_button_init_x = event->x;
  m_button_init_y = event->y;
  
  return true;
}

double Viewer::get_trackball_radius() {
  return get_width() < get_height() ? 
    (float)get_width() * 0.25 : (float)get_height() * 0.25;
}

void Viewer::draw_fps(double fps) {
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glColor3f(0.0, 0.0, 0.0);
  
  char fps_string[32];
  sprintf(fps_string, "%.2f", fps);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glRasterPos2f(-1.0f, 0.8f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ' ');
  for(unsigned int i=0; i < strlen(fps_string); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, fps_string[i]);
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

void Viewer::draw_score(int score) {
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glColor3f(0.0, 0.0, 0.0);
  
  char fps_string[32];
  sprintf(fps_string, "%d", score);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glRasterPos2f(-1.0f, 0.9f);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ' ');
  for(unsigned int i=0; i < strlen(fps_string); i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, fps_string[i]);
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

void Viewer::draw_trackball_circle()
{
  glActiveTexture(GL_TEXTURE0);
  glDisable(GL_TEXTURE_1D);
  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE2);
  glDisable(GL_TEXTURE_2D);

  int current_width = get_width();
  int current_height = get_height();
  
  // Set up for orthogonal drawing to draw a circle on screen.
  // You'll want to make the rest of the function conditional on
  // whether or not we want to draw the circle this time around.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, current_width, current_height);
  glOrtho(0.0, (float)current_width, 
           0.0, (float)current_height, -0.1, 0.1);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);

  // Reset modelview matrix
  glLoadIdentity();

  // draw a circle for use with the trackball 
  glDisable(GL_LIGHTING);
  glEnable(GL_LINE_SMOOTH);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glColor3d(1.0, 1.0, 1.0);
  double radius = get_trackball_radius();
  glTranslated((float)current_width * 0.5, (float)current_height * 0.5, 0);
  debug("begin");
  glBegin(GL_LINE_LOOP);
  for(size_t i=0; i<40; ++i) {
    double cosine = radius * cos(i*2*M_PI/40);
    double sine = radius * sin(i*2*M_PI/40);
    glVertex2f(cosine, sine);
  }
  debug("end");
  glEnd();
  //glColor3f(0.0, 0.0, 0.0);
  glDisable(GL_LINE_SMOOTH);
}

void Viewer::set_mode(int mode) {
  m_in_clicked_mode[0] = m_in_clicked_mode[1] = m_in_clicked_mode[2] = false;
  m_mode = mode;
}

void Viewer::reset_position() {
  m_translation = Vector3D();
}

void Viewer::reset_orientation() {
  m_rotation = Matrix4x4();
}

void Viewer::reset_joints() {
  // clear undo stack
  m_undo_items = std::vector<std::vector<UndoItem> >();
  m_undo_stack_pointer = -1;
  reset_all_joints();
}

void Viewer::reset_game() {
  m_game.restart();
}

void Viewer::reset_all() {
  reset_position();
  reset_orientation();
  reset_joints();
  reset_game();
}

const unsigned long SMOKE_INTERVAL = 150;

// try to render a new frame every 16 milliseconds
bool Viewer::on_game_clock_timeout() {
  static bool game_started = false;
  if (!game_started) {
    //std::cerr << "Starting game" << std::endl;
    m_game.start();
    game_started = true;
  }
  
  unsigned long time = get_time();
  static bool restarted = true;
  unsigned long time_since_start = (time - m_game.m_start_time) % m_game.m_length;
  // restart music at the beginning of the repetition so
  // it stays reasonably in sync.
  if (time_since_start < 30 && !restarted) {
    SM.PlayMusic(0);
    restarted = true;
  } else {
    restarted = false;
  }

  // change animation speed - only allowed to do it when it would
  // allow for reasonable continuity for all speed transitions.
  // i.e., if we say the length of a complete animation at full
  // speed is a 32nd of the total length of the song, and we
  // allow half speed, then we are only allowed to change when
  // these animations coincide (at the top), which is every
  // 16th of the total length of the song. 0 speed also
  // coincides at these moments since it never moves anywhere.
  long legal_offset = m_game.m_length / 16;
  long time_relative_to_offset = time_since_start % legal_offset;
  static bool changed = true;
  if (time_relative_to_offset < 40 ||
      (legal_offset - time_relative_to_offset) < 40) {
    if (!changed) {
      // it is legal to change animation speed
      if (m_game.m_score == 0) {
        set_anim_speed(0);
      } else if (m_game.m_score < 10) {
        set_anim_speed(2);
      } else {
        set_anim_speed(4);
      }
      changed = true;
    }
  } else {
    changed = false;
  }
  
  // update game state
  int missed = m_game.update();
  // if we missed a note then mute the music
  if (missed > 0) {
    SM.MuteMusic(0);
  }

  if (m_first) {
    draw((get_time() - m_game.m_start_time) % m_game.m_length);
  }
  m_first = false;

  if (m_camera_script) {
    // update camera position
    // the "3 *" is so that we can loop three different camera scripts, rather
    // than doing the same one every 25 seconds
    long camera_time_since_start = (time - m_game.m_start_time) % (3 * m_game.m_length);
    Animation* animation = m_camera.get_animation(camera_time_since_start);

    if (animation != NULL) {
      //if (last_animation == NULL
      double t = standard_easing((camera_time_since_start - animation->start_time) / double(animation->end_time - animation->start_time));
      Quat current_quat = (1 - t) * animation->initial_rotation + t * animation->final_rotation;
      current_quat.normalize();
      Vector3D current_translation = (1 - t) * animation->initial_position + t * animation->final_position;
      m_rotation = quat_to_rot(current_quat);
      m_translation = current_translation;
    }
  }

  static unsigned long next_smoke = 0;
  if (next_smoke < true_get_time()) {
    if (next_smoke == 0) {
      next_smoke = true_get_time() + SMOKE_INTERVAL;
    } else {
      next_smoke += SMOKE_INTERVAL;
    }
    m_root->shoot_particle(time_since_start, 2000, Point3D(0, 0, 0), Vector3D(0, 0.6, 0), Colour(0.4, 0.4, 0.4), 3, Matrix4x4());
  }

  invalidate();
  return true;
}

const int TIMEOUT_MILLIS = 200;

bool Viewer::on_failure_timeout() {
  m_failed_undo = false;
  invalidate();
  return false;
}

void Viewer::failed() {
  // give some indication of failure
  m_failed_undo = true;
  
  // set up momentum timer
  sigc::slot0<bool> my_slot = sigc::mem_fun(*this, &Viewer::on_failure_timeout);

  // This is where we connect the slot to the Glib::signal_timeout()
  sigc::connection conn = Glib::signal_timeout().connect(my_slot,
          TIMEOUT_MILLIS);
}

void Viewer::undo() {
  if (m_undo_stack_pointer == -1) {
    failed();
    invalidate();
    return;
  }
  std::vector<UndoItem> undo_items = m_undo_items[m_undo_stack_pointer--];
  //std::cerr << "m_undo_stack_pointer: " << m_undo_stack_pointer << std::endl;
  for (std::vector<UndoItem>::iterator it = undo_items.begin(); it != undo_items.end(); it++) {
    UndoItem undo_item = *it;
    JointNode* joint = (JointNode*) get_node(undo_item.m_id);
    //std::cerr << "setting back to x: " << undo_item.m_x << std::endl;
    joint->m_current_x = undo_item.m_x;
    joint->m_current_y = undo_item.m_y;
  }
  invalidate();
}

void Viewer::redo() {
  //std::cerr << "undo items: " << m_undo_items.size() << std::endl;
  //std::cerr << "m_undo_stack_pointer: " << m_undo_stack_pointer << std::endl;
  if (m_undo_stack_pointer >= (int) m_undo_items.size() - 2) {
    failed();
    invalidate();
    return;
  }
  m_undo_stack_pointer++;
  //std::cerr << "m_undo_stack_pointer: " << m_undo_stack_pointer << std::endl;
  std::vector<UndoItem> undo_items = m_undo_items[m_undo_stack_pointer + 1];
  for (std::vector<UndoItem>::iterator it = undo_items.begin(); it != undo_items.end(); it++) {
    UndoItem undo_item = *it;
    JointNode* joint = (JointNode*) get_node(undo_item.m_id);
    //std::cerr << "setting back to x: " << undo_item.m_x << std::endl;
    joint->m_current_x = undo_item.m_x;
    joint->m_current_y = undo_item.m_y;
  }
  invalidate();
}

void Viewer::toggle_circle() {
  m_circle = !m_circle;
}

void Viewer::toggle_zbuffer() {
  m_zbuffer = !m_zbuffer;
}

void Viewer::toggle_backface_cull() {
  m_backface_cull = !m_backface_cull;
}

void Viewer::toggle_frontface_cull() {
  m_frontface_cull = !m_frontface_cull;
}

void Viewer::toggle_fps() {
  m_fps = !m_fps;
}

void Viewer::toggle_show_light_depth() {
  m_show_light_depth = !m_show_light_depth;
}

void Viewer::toggle_camera_script() {
  m_camera_script = !m_camera_script;
}

void Viewer::toggle_draw_board() {
  m_draw_board = !m_draw_board;
}

void Viewer::toggle_compatibility() {
  m_compatibility = !m_compatibility;
}

void Viewer::toggle_reflections() {
  m_reflections = !m_reflections;
}

void Viewer::toggle_cel_shading() {
  m_cel_shading = !m_cel_shading;
}

void Viewer::toggle_outlines() {
  m_outlines = !m_outlines;
}

void Viewer::v_toggle_textures() {
  m_textures = !m_textures;
  toggle_textures();
  static bool first = true;
  if (!first) {
    m_root->redraw();
    m_first = true;
    m_camera_script = false;
    reset_all();
  }
  first = false;
}

void Viewer::v_toggle_slow_redraw() {
  toggle_slow_redraw();
}

void Viewer::toggle_shadows() {
  m_shadows = !m_shadows;
}

void Viewer::play_note(int note) {
  Note* hit = m_game.play_note(note);
  // hit is not NULL if we hit a note
  long elapsed = (get_time() - m_game.m_start_time) % m_game.m_length;
  if (hit != NULL) {
    SM.UnmuteMusic(0);
    // shower of stars should explode out of note
    Point3D start((note - 3) / 2.7, 0.1, 0);
    Colour colour(0, 0, 0);
    switch (note) {
      case 1:
        colour = Colour(0, 1, 0);
        break;
      case 2:
        colour = Colour(1, 0, 0);
        break;
      case 3:
        colour = Colour(1, 1, 0);
        break;
      case 4:
        colour = Colour(0, 0, 1);
        break;
      case 5:
        colour = Colour(1, 0.5, 0.25);
        break;
    }
    m_note_particles.create_particle(500, start, Vector3D(1, 2, 0), colour);
    m_note_particles.create_particle(500, start, Vector3D(-1, 2, 0), colour);
    m_note_particles.create_particle(500, start, Vector3D(0, 2, 1), colour);
    m_note_particles.create_particle(500, start, Vector3D(0, 2, -1), colour);
    if (m_compatibility) {
      m_root->shoot_particle(elapsed, 1000, Point3D(0, 0, 0), 6 * Vector3D(0, 0.8, 1.2), Colour(0.3, 0.3, 1), 1, Matrix4x4());
    } else {
      m_root->shoot_particle(elapsed, 1000, Point3D(0, 0, 0), 6 * Vector3D(0, 0.8, 1.2), colour, 1, Matrix4x4());
    }
  } else {
    SM.PlaySound(0);
    //SM.MuteMusic(0);
    if (m_compatibility) {
      m_root->shoot_particle(elapsed, 1000, Point3D(0, 0, 0), 6 * Vector3D(0, 0.8, 1.2), Colour(1, 0.3, 0.3), 1, Matrix4x4());
    } else {
      m_root->shoot_particle(elapsed, 1000, Point3D(0, 0, 0), 6 * Vector3D(0, 0.8, 1.2), Colour(1, 0, 0), 2, Matrix4x4());
    }
    
    // failed to hit a note!
    failed();
    invalidate();
  }
}

void Viewer::print_position() {
  Vector4D quat = rot_to_quat(m_rotation);
  std::cerr << quat[0] << "," << quat[1] << "," << quat[2] << "," << quat[3];
  std::cerr << "," << m_translation[0] << "," << m_translation[1] << "," << m_translation[2] << std::endl;
}
