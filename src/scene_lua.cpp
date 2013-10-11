//
// CS488 - Introduction to Computer Graphics
//
// scene_lua.cpp
//
// Everything that's needed to parse a scene file using Lua.
// You don't necessarily have to understand exactly everything that
// goes on here, although it will be useful to have a reasonable idea
// if you wish to add new commands to the scene format.
//
// Lua interfaces with C/C++ using a special stack. Everytime you want
// to get something from lua, or pass something back to lua (e.g. a
// return value), you need to use this stack. Thus, most of the lua_
// and luaL_ functions actually manipulate the stack. All the
// functions beginning with "lua_" are part of the Lua C API itself,
// whereas the "luaL_" functions belong to a library of useful
// functions on top of that called lauxlib.
//
// This file consists of a bunch of C function declarations which
// implement functions callable from Lua. There are also two tables
// used to set up the interface between Lua and these functions, and
// the main "driver" function, import_lua, which calls the lua
// interpreter and sets up all the state.
//
// Note that each of the function declarations follow the same format:
// they take as their only argument the current state of the lua
// interpreter, and return the number of values returned back to lua.
//
// For more information see the book "Programming In Lua," available
// online at http://www.lua.org/pil/, and of course the Lua reference
// manual at http://www.lua.org/manual/5.0/.
//
// http://lua-users.org/wiki/LauxLibDocumentation provides a useful
// documentation of the "lauxlib" functions (beginning with luaL_).
//
// -- University of Waterloo Computer Graphics Lab 2005

#include "scene_lua.hpp"
#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdio>
#include "lua488.hpp"
#include "mesh.hpp"
#include "modelanimation.hpp"

// Uncomment the following line to enable debugging messages
// #define GRLUA_ENABLE_DEBUG

#ifdef GRLUA_ENABLE_DEBUG
#  define GRLUA_DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#  define GRLUA_DEBUG_CALL do { std::cerr << __FUNCTION__ << std::endl; } while (0)
#else
#  define GRLUA_DEBUG(x) do { } while (0)
#  define GRLUA_DEBUG_CALL do { } while (0)
#endif

// You may wonder, for the following types, why we use special "_ud"
// types instead of, for example, just allocating SceneNodes directly
// from lua. Part of the answer is that Lua is a C api. It doesn't
// call any constructors or destructors for you, so it's easier if we
// let it just allocate a pointer for the node, and handle
// allocation/deallocation of the node itself. Another (perhaps more
// important) reason is that we will want SceneNodes to stick around
// even after lua is done with them, after all, we want to pass them
// back to the program. If we let Lua allocate SceneNodes directly,
// we'd lose them all when we are done parsing the script. This way,
// we can easily keep around the data, all we lose is the extra
// pointers to it.

// The "userdata" type for a node. Objects of this type will be
// allocated by Lua to represent nodes.
struct gr_node_ud {
  SceneNode* node;
};

// The "userdata" type for a material. Objects of this type will be
// allocated by Lua to represent materials.
struct gr_material_ud {
  Material* material;
};

// Create a node
extern "C"
int gr_node_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
  data->node = 0;

  const char* name = luaL_checkstring(L, 1);
  data->node = new SceneNode(name);

  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

// Create a particle node (particle emitter)
extern "C"
int gr_particle_emitter_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
  data->node = 0;

  luaL_checktype(L, 2, LUA_TNUMBER);

  const char* name = luaL_checkstring(L, 1);
  int id = (int) luaL_checknumber(L, 2);
  data->node = new ParticleNode(name, id);

  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

// Create a joint node
extern "C"
int gr_joint_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
  data->node = 0;

  const char* name = luaL_checkstring(L, 1);
  JointNode* node = new JointNode(name);

  luaL_checktype(L, 2, LUA_TTABLE);
  luaL_argcheck(L, luaL_getn(L, 2) == 3, 2, "Three-tuple expected");
  luaL_checktype(L, 3, LUA_TTABLE);
  luaL_argcheck(L, luaL_getn(L, 3) == 3, 3, "Three-tuple expected");

  double x[3], y[3];
  for (int i = 1; i <= 3; i++) {
    lua_rawgeti(L, 2, i);
    x[i - 1] = luaL_checknumber(L, -1);
    lua_rawgeti(L, 3, i);
    y[i - 1] = luaL_checknumber(L, -1);
    lua_pop(L, 2);
  }

  node->set_joint_x(x[0], x[1], x[2]);
  node->set_joint_y(y[0], y[1], y[2]);
  
  data->node = node;

  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

// Create a sphere node
extern "C"
int gr_sphere_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
  data->node = 0;
  
  const char* name = luaL_checkstring(L, 1);
  GeometryNode* gn = new GeometryNode(name, new Sphere());
  gn->set_material(NULL);
  data->node = gn;

  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

// Useful function to retrieve and check an n-tuple of numbers.
template<typename T>
void get_tuple(lua_State* L, int arg, T* data, int n)
{
  luaL_checktype(L, arg, LUA_TTABLE);
  luaL_argcheck(L, luaL_getn(L, arg) == n, arg, "N-tuple expected");
  for (int i = 1; i <= n; i++) {
    lua_rawgeti(L, arg, i);
    data[i - 1] = luaL_checknumber(L, -1);
    lua_pop(L, 1);
  }
}

// Create a polygonal mesh node
extern "C"
int gr_mesh_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)lua_newuserdata(L, sizeof(gr_node_ud));
  data->node = 0;

  const char* name = luaL_checkstring(L, 1);

  std::vector<Point3D> verts;
  std::vector< std::vector<int> > faces;

  luaL_checktype(L, 2, LUA_TTABLE);
  int vert_count = luaL_getn(L, 2);
  
  luaL_argcheck(L, vert_count >= 1, 2, "Tuple of vertices expected");

  for (int i = 1; i <= vert_count; i++) {
    lua_rawgeti(L, 2, i);

    Point3D vertex;
    get_tuple(L, -1, &vertex[0], 3);
    
    verts.push_back(vertex);
    lua_pop(L, 1);
  }

  luaL_checktype(L, 3, LUA_TTABLE);
  int face_count = luaL_getn(L, 3);
  
  luaL_argcheck(L, face_count >= 1, 3, "Tuple of faces expected");

  faces.resize(face_count);
  
  for (int i = 1; i <= face_count; i++) {
    lua_rawgeti(L, 3, i);

    luaL_checktype(L, -1, LUA_TTABLE);
    int index_count = luaL_getn(L, -1);

    luaL_argcheck(L, index_count >= 3, 3, "Tuple of indices expected");

    faces[i - 1].resize(index_count);
    get_tuple(L, -1, &faces[i - 1][0], index_count);
    
    lua_pop(L, 1);
  }

  Mesh* mesh = new Mesh(verts, faces);
  GRLUA_DEBUG(*mesh);
  data->node = new GeometryNode(name, mesh);

  luaL_getmetatable(L, "gr.node");
  lua_setmetatable(L, -2);

  return 1;
}

// Create a material
extern "C"
int gr_material_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_material_ud* data = (gr_material_ud*)lua_newuserdata(L, sizeof(gr_material_ud));
  data->material = 0;
  
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_argcheck(L, luaL_getn(L, 1) == 3, 1, "Three-tuple expected");
  luaL_checktype(L, 2, LUA_TTABLE);
  luaL_argcheck(L, luaL_getn(L, 2) == 3, 2, "Three-tuple expected");
  luaL_checktype(L, 3, LUA_TNUMBER);

  double kd[3], ks[3];
  for (int i = 1; i <= 3; i++) {
    lua_rawgeti(L, 1, i);
    kd[i - 1] = luaL_checknumber(L, -1);
    lua_rawgeti(L, 2, i);
    ks[i - 1] = luaL_checknumber(L, -1);
    lua_pop(L, 2);
  }
  double shininess = luaL_checknumber(L, 3);
  
  data->material = new PhongMaterial(Colour(kd[0], kd[1], kd[2]),
                                     Colour(ks[0], ks[1], ks[2]),
                                     shininess, NULL, 0);

  luaL_newmetatable(L, "gr.material");
  lua_setmetatable(L, -2);
  
  return 1;
}

// Create a material
extern "C"
int gr_textured_material_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_material_ud* data = (gr_material_ud*)lua_newuserdata(L, sizeof(gr_material_ud));
  data->material = 0;
  
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_argcheck(L, luaL_getn(L, 1) == 3, 1, "Three-tuple expected");
  luaL_checktype(L, 2, LUA_TTABLE);
  luaL_argcheck(L, luaL_getn(L, 2) == 3, 2, "Three-tuple expected");
  luaL_checktype(L, 3, LUA_TNUMBER);

  double kd[3], ks[3];
  for (int i = 1; i <= 3; i++) {
    lua_rawgeti(L, 1, i);
    kd[i - 1] = luaL_checknumber(L, -1);
    lua_rawgeti(L, 2, i);
    ks[i - 1] = luaL_checknumber(L, -1);
    lua_pop(L, 2);
  }
  double shininess = luaL_checknumber(L, 3);
  const char* texture_bmp = luaL_checkstring(L, 4);
  int setting = (int) luaL_checknumber(L, 5);
  
  data->material = new PhongMaterial(Colour(kd[0], kd[1], kd[2]),
                                     Colour(ks[0], ks[1], ks[2]),
                                     shininess, texture_bmp, setting);

  luaL_newmetatable(L, "gr.material");
  lua_setmetatable(L, -2);
  
  return 1;
}

// Add a child to a node
extern "C"
int gr_node_add_child_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;
  
  gr_node_ud* childdata = (gr_node_ud*)luaL_checkudata(L, 2, "gr.node");
  luaL_argcheck(L, childdata != 0, 2, "Node expected");

  SceneNode* child = childdata->node;

  self->add_child(child);

  return 0;
}

// Set a node's material
extern "C"
int gr_node_set_material_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  GeometryNode* self = dynamic_cast<GeometryNode*>(selfdata->node);

  luaL_argcheck(L, self != 0, 1, "Geometry node expected");
  
  gr_material_ud* matdata = (gr_material_ud*)luaL_checkudata(L, 2, "gr.material");
  luaL_argcheck(L, matdata != 0, 2, "Material expected");

  Material* material = matdata->material;

  self->set_material(material);

  return 0;
}

// Set a node's animations to slow as the player does worse
extern "C"
int gr_node_set_anim_slowable_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  self->set_anim_slowable();

  return 0;
}

// Set a node to have no outline
extern "C"
int gr_node_no_outline_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  GeometryNode* self = dynamic_cast<GeometryNode*>(selfdata->node);

  luaL_argcheck(L, self != 0, 1, "Geometry node expected");

  self->set_outline(false);

  return 0;
}

// Add an animation to a node (NOT a joint animation).
extern "C"
int gr_node_add_animation_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  long start_time = (long) luaL_checknumber(L, 2);
  long end_time = (long) luaL_checknumber(L, 3);
  luaL_checktype(L, 4, LUA_TTABLE);  
  Vector3D initial_position;
  get_tuple(L, 4, &initial_position[0], 3);
  luaL_checktype(L, 5, LUA_TTABLE);  
  Vector3D final_position;
  get_tuple(L, 5, &final_position[0], 3);

  self->add_animation(ModelAnimation(start_time, end_time, initial_position, final_position));

  return 0;
}

// Add a joint animation to a joint node.
extern "C"
int gr_node_add_joint_animation_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  JointNode* self = dynamic_cast<JointNode*>(selfdata->node);

  luaL_argcheck(L, self != 0, 1, "Joint node expected");

  long start_time = (long) luaL_checknumber(L, 2);
  long end_time = (long) luaL_checknumber(L, 3);
  luaL_checktype(L, 4, LUA_TTABLE);  
  double x[2];
  get_tuple(L, 4, &x[0], 2);
  luaL_checktype(L, 5, LUA_TTABLE);  
  double y[2];
  get_tuple(L, 5, &y[0], 2);

  self->add_joint_animation(JointAnimation(start_time, end_time, x[0], x[1], y[0], y[1]));

  return 0;
}

// Add a scaling transformation to a node.
extern "C"
int gr_node_scale_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  double values[3];
  
  for (int i = 0; i < 3; i++) {
    values[i] = luaL_checknumber(L, i + 2);
  }

  self->scale(Vector3D(values[0], values[1], values[2]));

  return 0;
}

// Add a translation to a node.
extern "C"
int gr_node_translate_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  double values[3];
  
  for (int i = 0; i < 3; i++) {
    values[i] = luaL_checknumber(L, i + 2);
  }

  self->translate(Vector3D(values[0], values[1], values[2]));

  return 0;
}

// Rotate a node.
extern "C"
int gr_node_rotate_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  SceneNode* self = selfdata->node;

  const char* axis_string = luaL_checkstring(L, 2);

  luaL_argcheck(L, axis_string
                && std::strlen(axis_string) == 1, 2, "Single character expected");
  char axis = std::tolower(axis_string[0]);
  
  luaL_argcheck(L, axis >= 'x' && axis <= 'z', 2, "Axis must be x, y or z");
  
  double angle = luaL_checknumber(L, 3);

  self->rotate(axis, angle);

  return 0;
}

// Garbage collection function for lua.
extern "C"
int gr_node_gc_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, data != 0, 1, "Node expected");

  // Note that we don't delete the node here. This is because we still
  // want the scene to be around when we close the lua interpreter,
  // but at that point everything will be garbage collected.
  //
  // If data->node happened to be a reference-counting pointer, this
  // will in fact just decrease lua's reference to it, so it's not a
  // bad thing to include this line.
  data->node = 0;

  return 0;
}

// This is where all the "global" functions in our library are
// declared.
// If you want to add a new non-member function, add it here.
static const luaL_reg grlib_functions[] = {
  {"node", gr_node_cmd},
  {"particle_emitter", gr_particle_emitter_cmd},
  {"joint", gr_joint_cmd},
  {"sphere", gr_sphere_cmd},
  {"textured_material", gr_textured_material_cmd},
  {"material", gr_material_cmd},
  {"mesh", gr_mesh_cmd},
  {0, 0}
};

// This is where all the member functions for "gr.node" objects are
// declared. Since all the other objects (e.g. materials) are so
// simple, we only really need to make member functions for nodes.
//
// If you want to add a new member function for gr.node, add it
// here.
//
// We could have used inheritance in lua to match the inheritance
// between different node types, but it's easier to just give all
// nodes the same Lua type and then do any additional type checking in
// the appropriate member functions (e.g. gr_node_set_material_cmd
// ensures that the node is a GeometryNode, see above).
static const luaL_reg grlib_node_methods[] = {
  {"__gc", gr_node_gc_cmd},
  {"add_child", gr_node_add_child_cmd},
  {"add_animation", gr_node_add_animation_cmd},
  {"add_joint_animation", gr_node_add_joint_animation_cmd},
  {"set_material", gr_node_set_material_cmd},
  {"anim_slowable", gr_node_set_anim_slowable_cmd},
  {"no_outline", gr_node_no_outline_cmd},
  {"scale", gr_node_scale_cmd},
  {"rotate", gr_node_rotate_cmd},
  {"translate", gr_node_translate_cmd},
  {0, 0}
};

// This function calls the lua interpreter to do the actual importing
SceneNode* import_lua(const std::string& filename)
{
  GRLUA_DEBUG("Importing scene from " << filename);
  
  // Start a lua interpreter
  lua_State* L = lua_open();

  GRLUA_DEBUG("Loading base libraries");
  
  // Load some base library
  luaL_openlibs(L);


  GRLUA_DEBUG("Setting up our functions");

  // Set up the metatable for gr.node
  luaL_newmetatable(L, "gr.node");
  lua_pushstring(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -3);

  // Load the gr.node methods
  luaL_openlib(L, 0, grlib_node_methods, 0);

  // Load the gr functions
  luaL_openlib(L, "gr", grlib_functions, 0);

  GRLUA_DEBUG("Parsing the scene");
  // Now parse the actual scene
  if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 1, 0)) {
    std::cerr << "Error loading " << filename << ": " << lua_tostring(L, -1) << std::endl;
    return 0;
  }

  GRLUA_DEBUG("Getting back the node");
  
  // Pull the returned node off the stack
  gr_node_ud* data = (gr_node_ud*)luaL_checkudata(L, -1, "gr.node");
  if (!data) {
    std::cerr << "Error loading " << filename << ": Must return the root node." << std::endl;
    return 0;
  }

  // Store it
  SceneNode* node = data->node;

  GRLUA_DEBUG("Closing the interpreter");
  
  // Close the interpreter, free up any resources not needed
  lua_close(L);

  // And return the node
  return node;
}
