#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <string>
#include <gtkmm.h>
#include "viewer.hpp"
#include "scene.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow(SceneNode* root, std::string filename);
  
private:
  void reload_puppet();
  void faster();
  void slower();
  void reset();

  // A "vertical box" which holds everything in our window
  Gtk::VBox m_vbox;

  // The menubar, with all the menus at the top of the window
  Gtk::MenuBar m_menubar;
  // Each menu itself
  Gtk::Menu m_menu_speed;
  Gtk::Menu m_menu_note;
  Gtk::Menu m_menu_app;
  Gtk::Menu m_menu_edit;
  Gtk::RadioMenuItem::Group m_menu_mode_group;
  Gtk::Menu m_menu_mode;
  Gtk::Menu m_menu_options;

  // The main OpenGL area
  Viewer m_viewer;

  std::string m_filename;
};

#endif
