#!/usr/bin/python

##
# A script to generate a saxophone model (or other series of cylinder-like
# thins)
##

import math
import sys

final = len(sys.argv) > 1

# computes the cross product of u = (a, b, c) and v = (d, e, f)
def cross(u, v):
  a = u[0]
  b = u[1]
  c = u[2]
  d = v[0]
  e = v[1]
  f = v[2]
  return (b * f - c * e, c * d - a * f, a * e - b * d)

# returns the length of u = (a, b, c)
def length(u):
  return math.sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2])

# returns a normalized version of u = (a, b, c)
def normalize(u):
  l = length(u)
  return (u[0] / l, u[1] / l, u[2] / l)

def plus(u, v):
  return (u[0] + v[0], u[1] + v[1], u[2] + v[2])

# multiply u = (a, b, c) by scalar s
def smult(s, u):
  return (u[0] * s, u[1] * s, u[2] * s)

# stupid tests
#print cross((1, 0, 0.5), (0, 0, 1))
#print normalize((3, 1, 2))

# the filename into which the mode should be written
filename = "sax.lua" if not final else "sax_final.lua"
# a series of vertices through which the centre of the saxophone
# should pass, along with the radius of the saxophone at each vertex,
# in ((x, y, z), r) tuples
paths = []
if False:
  path = [((0, 0, 0), 1), ((1, 0, 0), 1), ((2, -1, 0), math.sqrt(2)), ((2, -2, 0), 1)]
  paths.append((path, 1))
# generate a partial circle
if False:
  path = []
  segments = 500
  for segment in range(0, segments + 2):
    x = 4 * math.sin(math.pi * 2 * segment / segments)
    y = 4 * math.cos(math.pi * 2 * segment / segments)
    path.append(
      ((x, y, 0), math.sin(math.pi * 8 * segment / segments) + 1.5)
    )
  paths.append((path, 1))
if True:
  path = []
  reverse_path = []
  # these are bezier control points - each set of 4 represents one cubic
  # section. Generated with princeton cs426 bezier curve generator
  # because it was nice to work with.
  first = None
  subdivision = 15

  cps = [
    (202, 178, 0),
    (232, 183, 0),
    (255, 193, 0),
    (288, 179, 0),

    (288, 179, 0),
    (321, 165, 0),
    (331, 187, 0),
    (326, 222, 0),

    (326, 222, 0),
    (321, 257, 0),
    (304, 395, 0),
    (299, 427, 0),

    (299, 427, 0),
    (294, 459, 0),
    (305, 479, 0),
    (327, 479, 0),

    (327, 479, 0),
    (349, 479, 0),
    (357, 480, 0),
    (364, 436, 0),

    (364, 436, 0),
    (371, 392, 0),
    (379, 361, 0),
    (403, 342, 0)
  ]


  seen = {}
  r = 4.0
  last_value = 0
  for section in range(0, len(cps), 4):
    p1 = cps[section]
    p2 = cps[section + 1]
    p3 = cps[section + 2]
    p4 = cps[section + 3]
    for t_index in range(0, subdivision + 1):
      t = t_index / float(subdivision)
      c1 = (1 - t) * (1 - t) * (1 - t)
      c2 = 3.0 * t * (1 - t) * (1 - t)
      c3 = 3.0 * t * t * (1 - t)
      c4 = t * t * t
      x = p1[0] * c1 + p2[0] * c2 + p3[0] * c3 + p4[0] * c4
      y = p1[1] * c1 + p2[1] * c2 + p3[1] * c3 + p4[1] * c4
      q_t = (x, y, 0)
      if first is None:
        first = q_t
      q_t = plus(q_t, smult(-1, first))
      q_t = (q_t[0], -q_t[1], q_t[2])
      #print q_t
      if (not q_t in seen):
        # make radius linearly increase
        #r = 4 + 4 * ((section / 4.0) + t)
        value = (section / 4.0 + t)
        diff = value - last_value
        last_value = value
        m = 4
        if section >= len(cps) - 8:
          #print section
          m = 0
        if section >= len(cps) - 4:
          m = 0
          if t >= 0.4:
            m = 4
          if t >= 0.7:
            m += (35 * (t - 0.7)) ** 2
        r += m * diff
        path.append((q_t, r))
        if section >= len(cps) - 4 and t_index == subdivision:
          reverse_path.append((q_t, r))
        elif section >= len(cps) - 4:
          reverse_path.append((q_t, r * (1 - ((subdivision - t_index) / float(subdivision))) * 7 / 8.0))
        seen[q_t] = True
  paths.append((path, 0.01, False))
  #d = normalize(plus(reverse_path[-1][0], smult(-1, reverse_path[-2][0])))
  # total failure
  #reverse_path[-1] = (plus(reverse_path[-1][0], smult(-2, d)), reverse_path[-1][1])
  paths.append((reverse_path, 0.01, True))

#print path

# the number of sides that each cylinder piece should have
sides = 20

f = open(filename, 'w')
if not final: f.write("rootnode = gr.node('root')\n")

# write the material to use
f.write("sax_gold = gr.material({0.99609375, 0.796875, 0.19921875}, {0.1, 0.1, 0.1}, 10)\n")

for i, (path, scale, reverse) in enumerate(paths):
  # generate the saxophone
  last_vertices = None
  last_centre = None
  last_radius = None
  # a list of vertices
  vertices = []
  # a mapping of vertices to a unique index
  vertex_map = {}
  # a list of faces, each of which is a list of indices into the vertex list
  faces = []

  for segment in path:
    centre = segment[0]
    radius = segment[1]

    point_vertices = []
    if (not (last_centre is None or last_vertices is None or last_radius is None)):
      point_vertices = []
      # the direction
      d = (centre[0] - last_centre[0], centre[1] - last_centre[1], centre[2] - last_centre[2])
      for side in range(0, sides):
        # perpendicular direction (to z axis and d)
        pd = normalize(smult(-1, cross(d, (0, 0, 1))))
        # calculate components that will make up final direction
        # the component coming from d
        pd_comp = smult(radius * math.sin(2 * math.pi * side / sides), pd)
        z_comp = smult(radius * math.cos(2 * math.pi * side / sides), (0, 0, 1))
        vertex = plus(pd_comp, plus(z_comp, centre))
        if reverse:
          vertex = plus(smult(5, normalize(d)), vertex)
        point_vertices.append(vertex)

      # add in the new vertices to the list as necessary
      for vertex in point_vertices:
        if not vertex in vertex_map:
          vertices.append(vertex)
          vertex_map[vertex] = len(vertices) - 1

      # generate the faces
      for side in range(0, sides):
        faces.append([
          vertex_map[point_vertices[side]],
          vertex_map[point_vertices[(side + 1) % sides]],
          vertex_map[last_vertices[(side + 1) % sides]],
          vertex_map[last_vertices[side]]])
    else:
      for side in range(0, sides):
        # calculate y and z
        y = radius * math.sin(2 * math.pi * side / sides)
        z = radius * math.cos(2 * math.pi * side / sides)
        vertex = (centre[0], centre[1] + y, centre[2] + z)
        vertices.append(vertex)
        vertex_map[vertex] = len(vertices) - 1
        point_vertices.append(vertex);
        
    last_centre = centre
    last_vertices = point_vertices
    last_radius = radius

  # write out mesh
  f.write("sax%d = gr.mesh('sax', {\n" % (i))
  # write vertices
  f.write(",\n".join(
    [("  {" + (", ".join(map(str, vertex))) + "}")
      for vertex in vertices]
  ))
  f.write("}, {\n")
  # write faces
  f.write(",\n".join(
    [("  {" + (", ".join(map(str, reversed(face) if reverse else face))) + "}")
      for face in faces]
  ))
  f.write("})\n")
  if not final: f.write("rootnode:add_child(sax%d)\n" % (i))
  if not final: f.write("sax%d:scale(%f, %f, %f)\n" % (i, scale, scale, scale))
  f.write("sax%d:set_material(sax_gold)\n" % (i))

if False:
  f.write("""
  well_top = gr.mesh('well_top', {
    {1, 0, 1},
    {-1, 0, 1},
    {1, 0, -1},
    {-1, 0, -1},
    {1, 1, 0},
    {-1, 1, 0}
  }, {
    {0, 1, 3, 2},
    {0, 2, 4},
    {5, 3, 1},
    {4, 5, 1, 0},
    {2, 3, 5, 4}
  })
  rootnode:add_child(well_top)
  well_top:set_material(red)
  well_top:translate(0, 0, 0)
  well_top:scale(1.1, 0.5, 0.75)
  """)

if not final: f.write("rootnode:translate(0, 0, -10.0)\n")
if not final: f.write("return rootnode\n")
