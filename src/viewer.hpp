#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include "scene.hpp"
#include <set>
#include <vector>
#include "pass.hpp"
#include "game.hpp"
#include "scene_lua.hpp"
#include "particles.hpp"
#include "camera.hpp"

const int MODE_POSITION = 1;
const int MODE_JOINTS = 2;
const int MODE_LIGHT = 3;
const int MODE_SHADOW = 4;

class UndoItem {
public:
  UndoItem(int id, double x, double y);
  virtual ~UndoItem();

  int m_id;
  double m_x;
  double m_y;
};

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer(SceneNode* root);
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();

  // reload puppet model
  void load(SceneNode* root);

  void set_mode(int new_mode);
  void reset_position();
  void reset_orientation();
  void reset_joints();
  void reset_game();
  void reset_all();
  void print_position();
  void undo();
  void redo();
  void toggle_circle();
  void toggle_zbuffer();
  void toggle_backface_cull();
  void toggle_frontface_cull();
  void toggle_fps();
  void toggle_camera_script();
  void toggle_draw_board();
  void toggle_compatibility();
  void toggle_reflections();
  void toggle_cel_shading();
  void toggle_outlines();
  void v_toggle_textures();
  void v_toggle_slow_redraw();
  // light/shadow
  void toggle_show_light_depth();
  void toggle_shadows();
  void play_music();
  void play_note(int note);

protected:

  // Events we implement
  // Note that we could use gtkmm's "signals and slots" mechanism
  // instead, but for many classes there's a convenient member
  // function one just needs to define that'll be called with the
  // event.

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);

  // Draw a circle for the trackball, with OpenGL commands.
  // Assumes the context for the viewer is active.
  void draw_trackball_circle();
  
private:
  void failed();
  void draw_inner(long time, bool reflected);
  void setup_outline(bool reflected);
  void teardown_outline(bool reflected);
  void initialize_shadow_map();
  void draw_fps(double fps);
  void draw_score(int score);
  void draw_board(pass_t pass);
  void do_model_transformation();
  void add_undo_stack_entry();
  bool on_game_clock_timeout();
  bool on_failure_timeout();
  void draw(long time);
  void actually_draw(Point3D light_position, long time, pass_t pass);
  void draw_particles(pass_t pass, bool reflected, unsigned long time);
  bool select(int x, int y);
  double get_trackball_radius();

  double m_rolling_fps;

  Game m_game;
  Camera m_camera;
  Particles m_note_particles;
  SceneNode* m_note;
  SceneNode* m_top;

  bool m_failed_undo;
  bool m_something_changed;
  std::vector<std::vector<UndoItem> > m_undo_items;
  int m_undo_stack_pointer;

  Point3D m_light_position;
  Point3D m_light_look_at[5];
  SceneNode* m_root;
  std::set<int> m_picked_nodes;

  // position
  Vector3D m_translation;
  Matrix4x4 m_rotation;

  // CONTROL VARIABLES
  int m_mode;

  bool m_circle;
  bool m_zbuffer;
  bool m_backface_cull;
  bool m_frontface_cull;
  bool m_fps;
  bool m_camera_script;
  bool m_draw_board;
  bool m_compatibility;
  bool m_reflections;
  bool m_cel_shading;
  bool m_outlines;
  bool m_textures;
  bool m_first;

  // shadow parameters
  bool m_shadows;
  bool m_show_light_depth;
  double m_light_fov;
  double m_light_near;
  double m_light_far;
  double m_bias;
  int m_direction_viewed;

  // button variables for mouse controls
  int m_button_init_x;
  int m_button_init_y;
  bool m_buttons[3];
  bool m_in_clicked_mode[3];
};

#endif
