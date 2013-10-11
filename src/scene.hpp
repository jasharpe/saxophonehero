#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <set>
#include <vector>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "pass.hpp"
#include "particles.hpp"
#include "modelanimation.hpp"

void toggle_slow_redraw();

void set_anim_speed(int speed);

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(Point3D light_position, long time, std::set<int>& picked_nodes, pass_t pass, bool picking);
  virtual void shoot_particle(long time, int TTL, Point3D position, Vector3D velocity, Colour colour, int particle_id, Matrix4x4 trans);
  virtual void draw_particles(pass_t pass, bool reflected, unsigned long time, Matrix4x4 rotation_matrix);
  virtual void redraw();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
 
  void add_animation(ModelAnimation animation) {
    m_max_animation_end = std::max(m_max_animation_end, animation.end_time);
    m_animations.push_back(animation);
  }

  Vector3D apply_animations(long time);

  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_anim_slowable() {
    m_anim_slowable = true;
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    child->m_parent = this;
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  virtual double rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;

  Vector3D get_translation();
  
  // Useful for picking
  int m_id;
  std::string m_name;
  
  // need to know this so that we know whether the parent is
  // a joint or not.
  SceneNode* m_parent;

protected:

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
  Vector3D m_translation;
  long m_max_animation_end;
  std::vector<ModelAnimation> m_animations;

  bool m_anim_slowable;
};

SceneNode* get_node(int id);

void reset_all_joints();

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(Point3D light_position, long time, std::set<int>& picked_nodes, pass_t pass, bool picking);

  virtual void shoot_particle(long time, int TTL, Point3D position, Vector3D velocity, Colour colour, int particle_id, Matrix4x4 trans);

  void add_joint_animation(JointAnimation animation) {
    m_max_joint_animation_end = std::max(m_max_joint_animation_end, animation.end_time);
    m_joint_animations.push_back(animation);
  }

  Matrix4x4 apply_joint_animations(long time);

  virtual double rotate(char axis, double angle);

  virtual bool is_joint() const;

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  void reset();

  double m_current_x, m_current_y;
protected:

  int m_how_many;
  JointRange m_joint_x, m_joint_y;
  long m_max_joint_animation_end;
  std::vector<JointAnimation> m_joint_animations;
};

std::vector<JointNode*>* get_all_joints();

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();
  
  virtual void walk_gl(Point3D light_position, long time, std::set<int>& picked_nodes, pass_t pass, bool picking);
  virtual void redraw();

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

  void set_outline(bool outline) {
    m_outline = outline;
  }

protected:
  Material* m_material;
  Primitive* m_primitive;

private:
  bool m_outline;
};

class ParticleNode : public SceneNode {
public:
  ParticleNode(const std::string& name, int particle_id);
  virtual ~ParticleNode();

  virtual void shoot_particle(long time, int TTL, Point3D position, Vector3D velocity, Colour colour, int particle_id, Matrix4x4 trans);

  virtual void draw_particles(pass_t pass, bool reflected, unsigned long time, Matrix4x4 rotation_matrix);

protected:
  Particles m_particles;
  int m_particle_id;
};

#endif
