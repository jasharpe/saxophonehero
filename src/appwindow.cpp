#include "appwindow.hpp"
#include "scene_lua.hpp"
#include "time.hpp"

AppWindow::AppWindow(SceneNode* root, std::string filename) : m_viewer(root), m_filename(filename)
{
  set_title("Saxophone Hero");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  using Gtk::Menu_Helpers::RadioMenuElem;
  using Gtk::Menu_Helpers::CheckMenuElem;
  
  // letters: 67

  // SUCH a hack, but set up a menu for key presses since masking button
  // presses breaks this and I don't have time to figure out how to
  // avoid this properly.
  sigc::slot1<void, int> play_note_slot = 
  	sigc::mem_fun(m_viewer, &Viewer::play_note);

  m_menu_note.items().push_back(MenuElem("_Green", Gtk::AccelKey("1"),
    sigc::bind(play_note_slot, 1)));
  m_menu_note.items().push_back(MenuElem("_Red", Gtk::AccelKey("2"),
    sigc::bind(play_note_slot, 2)));
  m_menu_note.items().push_back(MenuElem("_Yellow", Gtk::AccelKey("3"),
    sigc::bind(play_note_slot, 3)));
  m_menu_note.items().push_back(MenuElem("_Blue", Gtk::AccelKey("4"),
    sigc::bind(play_note_slot, 4)));
  m_menu_note.items().push_back(MenuElem("_Orange", Gtk::AccelKey("5"),
    sigc::bind(play_note_slot, 5)));

  m_menu_speed.items().push_back(MenuElem("_Slower", Gtk::AccelKey("8"),
    sigc::mem_fun(*this, &AppWindow::slower)));
  m_menu_speed.items().push_back(MenuElem("_Reset", Gtk::AccelKey("9"),
    sigc::mem_fun(*this, &AppWindow::reset)));
  m_menu_speed.items().push_back(MenuElem("_Faster", Gtk::AccelKey("0"),
    sigc::mem_fun(*this, &AppWindow::faster)));

  // Set up the application menu
  m_menu_app.items().push_back(MenuElem("Reset Pos_ition", Gtk::AccelKey("i"),
    sigc::mem_fun(m_viewer, &Viewer::reset_position)));
  m_menu_app.items().push_back(MenuElem("Reset _Orientation", Gtk::AccelKey("o"),
    sigc::mem_fun(m_viewer, &Viewer::reset_orientation)));
  m_menu_app.items().push_back(MenuElem("Reset Joi_nts", Gtk::AccelKey("n"),
    sigc::mem_fun(m_viewer, &Viewer::reset_joints)));
  m_menu_app.items().push_back(MenuElem("Reset _Game", Gtk::AccelKey("g"),
    sigc::mem_fun(m_viewer, &Viewer::reset_game)));
  m_menu_app.items().push_back(MenuElem("Reset _All", Gtk::AccelKey("a"),
    sigc::mem_fun(m_viewer, &Viewer::reset_all)));
  m_menu_app.items().push_back(MenuElem("Print _Position", Gtk::AccelKey("k"),
    sigc::mem_fun(m_viewer, &Viewer::print_position)));
  // leaving this out since it's kind of unstable. Good for testing but can
  // mess a lot of stuff up.
  m_menu_app.items().push_back(MenuElem("R_eload Puppet", Gtk::AccelKey("e"),
    sigc::mem_fun(*this, &AppWindow::reload_puppet)));
  // The slot we use here just causes AppWindow::hide() on this,
  // which shuts down the application.
  m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));
  
  // set up the edit menu
  m_menu_edit.items().push_back(MenuElem("_Undo", Gtk::AccelKey("u"),
    sigc::mem_fun(m_viewer, &Viewer::undo)));
  m_menu_edit.items().push_back(MenuElem("_Redo", Gtk::AccelKey("r"),
    sigc::mem_fun(m_viewer, &Viewer::redo)));
  // this is dangerous and not really necessary since the music does
  // a pretty good job of resyncing itself. Accidentally clicking this
  // really sucks!
  //m_menu_edit.items().push_back(MenuElem("Play _Music", Gtk::AccelKey("m"),
  //  sigc::mem_fun(m_viewer, &Viewer::play_music)));

  // set up the mode menu
  sigc::slot1<void, int> mode_slot = 
  	sigc::mem_fun(m_viewer, &Viewer::set_mode);
  
  m_menu_mode.items().push_back(RadioMenuElem(m_menu_mode_group,
    "_Position", Gtk::AccelKey("p"),
    sigc::bind(mode_slot, MODE_POSITION)));
  m_menu_mode.items().push_back(RadioMenuElem(m_menu_mode_group,
    "_Joints", Gtk::AccelKey("j"),
    sigc::bind(mode_slot, MODE_JOINTS)));
  m_menu_mode.items().push_back(RadioMenuElem(m_menu_mode_group,
    "_Light", Gtk::AccelKey("l"),
    sigc::bind(mode_slot, MODE_LIGHT)));
  m_menu_mode.items().push_back(RadioMenuElem(m_menu_mode_group,
    "_Shadow", Gtk::AccelKey("s"),
    sigc::bind(mode_slot, MODE_SHADOW)));
  
  // set up the options menu
  m_menu_options.items().push_back(CheckMenuElem("_Circle", Gtk::AccelKey("c"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_circle)));
  m_menu_options.items().push_back(CheckMenuElem("_Z-buffer", Gtk::AccelKey("z"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_zbuffer)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[1]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("_Backface Cull", Gtk::AccelKey("b"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_backface_cull)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[2]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("_Frontface Cull", Gtk::AccelKey("f"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_frontface_cull)));
  m_menu_options.items().push_back(CheckMenuElem("F_PS", Gtk::AccelKey("x"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_fps)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[4]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("Light _Depth Buffer", Gtk::AccelKey("d"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_show_light_depth)));
  m_menu_options.items().push_back(CheckMenuElem("Shado_ws", Gtk::AccelKey("w"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_shadows)));
  //dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[6]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("C_amera Script", Gtk::AccelKey("v"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_camera_script)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[7]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("_Draw Board", Gtk::AccelKey("h"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_draw_board)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[8]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("_Compatibility", Gtk::AccelKey("t"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_compatibility)));
  //dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[9]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("_Slow Redraw", Gtk::AccelKey("y"),
    sigc::mem_fun(m_viewer, &Viewer::v_toggle_slow_redraw)));
  m_menu_options.items().push_back(CheckMenuElem("_Reflections", Gtk::AccelKey("<control>r"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_reflections)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[11]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("Cel Shading", Gtk::AccelKey("<control>c"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_cel_shading)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[12]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("Outlines", Gtk::AccelKey("<control>o"),
    sigc::mem_fun(m_viewer, &Viewer::toggle_outlines)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[13]).set_active();
  m_menu_options.items().push_back(CheckMenuElem("Textures", Gtk::AccelKey("<control>t"),
    sigc::mem_fun(m_viewer, &Viewer::v_toggle_textures)));
  dynamic_cast<Gtk::CheckMenuItem&>(m_menu_options.items()[14]).set_active();

  // Set up the menu bar
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Edit", m_menu_edit));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", m_menu_mode));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Options", m_menu_options));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Speed", m_menu_speed));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Note", m_menu_note));
  
  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(m_vbox);

  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  m_viewer.set_size_request(512, 512);
  m_vbox.pack_start(m_viewer);

  //add_events(Gdk::KEY_RELEASE_MASK);

  show_all();
}

void AppWindow::faster() {
  change_speed(1);
}

void AppWindow::slower() {
  change_speed(-1);
}

void AppWindow::reset() {
  reset_speed();
}

void AppWindow::reload_puppet() {
  SceneNode* root = import_lua(m_filename);
  if (!root) {
    std::cerr << "Could not open " << m_filename << std::endl;
  } else {
    m_viewer.load(root);
  }
}
