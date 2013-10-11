#include "scene.hpp"
#include <iostream>
#include "a3.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <map>
#include <utility>
#include "checkerror.hpp"
#include "easing.hpp"

bool slow_redraw = false;
int anim_speed = 4;

void set_anim_speed(int speed) {
  anim_speed = speed;
}

void toggle_slow_redraw() {
  slow_redraw = !slow_redraw;
}

std::map<int, SceneNode*> node_map = std::map<int, SceneNode*>();
std::vector<JointNode*> all_joints = std::vector<JointNode*>();

SceneNode* get_node(int id) {
  std::map<int, SceneNode*>::iterator iter =
    node_map.find(id);
  if (iter != node_map.end()) {
    return iter->second;
  }
  return NULL;
}

std::vector<JointNode*>* get_all_joints() {
  return &all_joints;
}

void reset_all_joints() {
  for (std::map<int, SceneNode*>::iterator it = node_map.begin(); it != node_map.end(); it++) {
    SceneNode* node = it->second;
    if (node->is_joint()) {
      JointNode* jnode = (JointNode*) node;
      jnode->reset();
    }
  }
}

SceneNode::SceneNode(const std::string& name)
  : m_name(name), m_max_animation_end(-1), m_anim_slowable(false)
{
  static int next_id = 0;
  m_id = next_id++;
  node_map.insert(std::make_pair(m_id, this));
}

SceneNode::~SceneNode()
{
}

Vector3D SceneNode::apply_animations(long time) {
  if (m_max_animation_end == -1) {
    return Vector3D(0, 0, 0);
  }
  long search_time;
  if (m_anim_slowable) {
    search_time = ((anim_speed * time) / 4) % m_max_animation_end;
  } else {
    search_time = time % m_max_animation_end;
  }
  ModelAnimation* to_apply = NULL;
  for (unsigned int i = 0; i < m_animations.size(); i++) {
    ModelAnimation* animation = &m_animations[i];
    if (animation->start_time <= search_time && animation->end_time >= search_time) {
      to_apply = animation;
      break;
    }
  }
  if (to_apply != NULL) {
    // apply animation's translation
    double t = standard_easing((search_time - to_apply->start_time) / double(to_apply->end_time - to_apply->start_time));
    Vector3D current_position = (1 - t) * to_apply->initial_position + t * to_apply->final_position;
    return Vector3D(current_position[0] - m_translation[0], current_position[1] - m_translation[1], current_position[2] - m_translation[2]);
  }
  return Vector3D(0, 0, 0);
}

void SceneNode::walk_gl(Point3D light_position, long time, std::set<int>& picked_nodes, pass_t pass, bool picking)
{
  glPushMatrix();
  Vector3D position = apply_animations(time);
  glTranslated(position[0], position[1], position[2]);
  double vals[16];
  m_trans.dump(vals);
  glMultMatrixd(vals);
  // traverse children
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->walk_gl(light_position, time, picked_nodes, pass, picking);
  }
  glPopMatrix();
}

void SceneNode::shoot_particle(long time, int TTL, Point3D position, Vector3D velocity, Colour colour, int particle_id, Matrix4x4 trans) {
  Vector3D anim_position = apply_animations(time);
  Matrix4x4 trans_trans = trans * translation(anim_position) * m_trans;
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->shoot_particle(time, TTL, position, velocity, colour, particle_id, trans_trans);
  }
}

void SceneNode::draw_particles(pass_t pass, bool reflected, unsigned long time, Matrix4x4 rotation_matrix) {
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->draw_particles(pass, reflected, time, rotation_matrix);
  }
}

void SceneNode::redraw() {
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->redraw();
  }
}

double SceneNode::rotate(char axis, double angle)
{
  //std::cerr << "Stub: Rotate " << m_name << " around " << axis << " by " << angle << std::endl;
  set_transform(m_trans * rotation(angle, axis));
  return angle;
}

void SceneNode::scale(const Vector3D& amount)
{
  //std::cerr << "Stub: Scale " << m_name << " by " << amount << std::endl;
  set_transform(m_trans * scaling(amount));
}

Vector3D SceneNode::get_translation() {
  return m_translation;
}

void SceneNode::translate(const Vector3D& amount)
{
  m_translation = m_translation + amount;
  //std::cerr << "Stub: Translate " << m_name << " by " << amount << std::endl;
  set_transform(m_trans * translation(amount));
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
  m_max_joint_animation_end = -1;
  m_current_x = 0;
  m_current_y = 0;
  m_how_many = 0;
  all_joints.push_back(this);
}

JointNode::~JointNode()
{
}

void JointNode::reset() {
  m_current_x = m_joint_x.init;
  m_current_y = m_joint_y.init;
}

Matrix4x4 JointNode::apply_joint_animations(long time) {
  if (m_max_joint_animation_end == -1) {
    return Matrix4x4();
  }
  long search_time;
  if (m_anim_slowable) {
    search_time = ((anim_speed * time) / 4) % m_max_joint_animation_end;
  } else {
    search_time = time % m_max_joint_animation_end;
  }
  JointAnimation* to_apply = NULL;
  for (unsigned int i = 0; i < m_joint_animations.size(); i++) {
    JointAnimation* animation = &m_joint_animations[i];
    if (animation->start_time <= search_time && animation->end_time >= search_time) {
      to_apply = animation;
      break;
    }
  }
  if (to_apply != NULL) {
    // apply animation's translation
    double t = standard_easing((search_time - to_apply->start_time) / double(to_apply->end_time - to_apply->start_time));
    if (m_id == 127) {
    //std::cerr << "search time: " << search_time << ", to apply: " << to_apply->start_time << ", t: " << t << std::endl;
  }
    double x = (1 - t) * to_apply->initial_x + t * to_apply->final_x;
    double y = (1 - t) * to_apply->initial_y + t * to_apply->final_y;
    if (m_how_many == 0) {
      if (slow_redraw) {
        redraw();
      }
    }
    //m_how_many = (m_how_many + 1) % 13;
    return rotation(y, 'y') * rotation(x, 'x') * rotation(m_current_x, 'x').invert() * rotation(m_current_y, 'y').invert();
  }
  return Matrix4x4();
}

void JointNode::walk_gl(Point3D light_position, long time, std::set<int>& picked_nodes, pass_t pass, bool picking)
{
  // do something with joint x and joint y?
  glPushMatrix();  
  Vector3D position = apply_animations(time);
  Matrix4x4 rot = apply_joint_animations(time);

  // move joint into position
  Matrix4x4 trans = translation(position) * translation(m_translation) *
    rot *
    rotation(m_current_y, 'y') *  
    rotation(m_current_x, 'x');
  double vals[16];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      vals[i + 4*j] = trans[i][j];
    }
  }
  glMultMatrixd(vals);
  // traverse children
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->walk_gl(light_position, time, picked_nodes, pass, picking);
  }
  glPopMatrix();
}

void JointNode::shoot_particle(long time, int TTL, Point3D position, Vector3D velocity, Colour colour, int particle_id, Matrix4x4 trans) {
  Vector3D anim_position = apply_animations(time);
  Matrix4x4 rot = apply_joint_animations(time);

  Matrix4x4 trans_trans = trans *
    translation(anim_position) *
    translation(m_translation) *
    rot *
    rotation(m_current_y, 'y') *  
    rotation(m_current_x, 'x');
  
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->shoot_particle(time, TTL, position, velocity, colour, particle_id, trans_trans);
  }
}

double JointNode::rotate(char axis, double angle) {
  //std::cerr << "Rotating " << angle << " about " << axis << std::endl;
  if (axis == 'x') {
    if (m_current_x + angle > m_joint_x.max) {
      angle = m_joint_x.max - m_current_x;
    } else if (m_current_x + angle < m_joint_x.min) {
      angle = m_joint_x.min - m_current_x;
    }
    m_current_x += angle;
    std::cerr << "m_id: " << m_id << ", axis: " << axis << ", angle: " << m_current_x << std::endl;
  } else if (axis == 'y') {
    if (m_current_y + angle > m_joint_y.max) {
      angle = m_joint_y.max - m_current_y;
    } else if (m_current_y + angle < m_joint_y.min) {
      angle = m_joint_y.min - m_current_y;
    }
    m_current_y += angle;
    std::cerr << "m_id: " << m_id << ", axis: " << axis << ", angle: " << m_current_y << std::endl;
  } else {
    // should not do this, joints should only rotate in
    // x and y directions. In theory they could work in
    // z too, but I haven't supported it.
    std::cerr << "DON'T ROTATE JOINTS IN Z!!!" << std::endl;
  }

  redraw();
  
  return angle;
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
  m_current_x = init;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
  m_current_y = init;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive),
    m_outline(true)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::walk_gl(Point3D light_position, long time, std::set<int>& picked_nodes, pass_t pass, bool picking)
{
  glPushMatrix();
  Vector3D position = apply_animations(time);
  glTranslated(position[0], position[1], position[2]);
  double vals[16];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      vals[i + 4*j] = m_trans[i][j];
    }
  }
  glMultMatrixd(vals);

  int texture_setting = 0;
  GLuint texture = 0;
  if (m_material != NULL) {
    m_material->init();
    if (pass == NORMAL || pass == SHADED) {
      //std::cerr << m_material << std::endl;
      m_material->apply_gl(pass, set_contains2(picked_nodes, m_id));
    }
    texture = m_material->texture();
    texture_setting = m_material->m_setting;
  }
  if (m_parent != NULL && m_parent->is_joint()) {
    glLoadName(m_id);
  }
  if (pass != OUTLINE || (pass == OUTLINE && m_outline)) {
    check_error(23523);
    //std::cerr << "m_id: " << m_id << std::endl;
    m_primitive->walk_gl(light_position, picking, texture, texture_setting, pass, set_contains2(picked_nodes, m_id));
    check_error(8117);
  }
  if (m_parent != NULL && m_parent->is_joint()) {
    glLoadName(-1);
  }
  // traverse children
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->walk_gl(light_position, time, picked_nodes, pass, picking);
  }
  glPopMatrix();
}

void GeometryNode::redraw() {
  m_primitive->redraw();
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->redraw();
  }
}

ParticleNode::ParticleNode(const std::string& name, int particle_id)
  : SceneNode(name), m_particles(SAX_END), m_particle_id(particle_id) {
  if (particle_id == 2) {
    m_particles = Particles(SAX_END2);
  } else if (particle_id == 3) {
    m_particles = Particles(SMOKE);
  }

}

ParticleNode::~ParticleNode() {
}

void ParticleNode::shoot_particle(long time, int TTL, Point3D position, Vector3D velocity, Colour colour, int particle_id, Matrix4x4 trans) {
  Vector3D anim_position = apply_animations(time);
  Matrix4x4 trans_trans = trans * translation(anim_position) * m_trans;
  if (particle_id == m_particle_id) {
    //std::cerr << "Shooting particle!" << std::endl;
    Point3D trans_position = trans_trans * position;
    Vector3D trans_velocity = trans_trans * velocity;
    if (m_particles.m_physics == SMOKE) {
      trans_velocity = velocity;
    }
    trans_velocity.normalize();
    trans_velocity = velocity.length() * trans_velocity;
    m_particles.create_particle(TTL, trans_position, trans_velocity, colour);
  } else {
    for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
      (*it)->shoot_particle(time, TTL, position, velocity, colour, particle_id, trans_trans);
    }
  }
}

void ParticleNode::draw_particles(pass_t pass, bool reflected, unsigned long time, Matrix4x4 rotation_matrix) {
  m_particles.draw(pass, reflected, time, rotation_matrix);
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->draw_particles(pass, reflected, time, rotation_matrix);
  }
}
