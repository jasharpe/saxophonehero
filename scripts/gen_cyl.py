#!/usr/bin/python

##
# A script to generate a cylinder mesh using quads
##

import math
import sys

final = len(sys.argv) > 1

sides = 30
filename = "cyl_test.lua" if not final else "cyl.lua"

f = open(filename, 'w')
if not final:
  f.write("rootnode = gr.node('root')\n")
  f.write("test_mat = gr.material({0.99609375, 0.796875, 0.19921875}, {0.1, 0.1, 0.1}, 10)\n")

# generate cylinder
top_centre = (0, 1, 0)
bottom_centre = (0, -1, 0)
vertex_map = {top_centre : 0, bottom_centre : 1}
vertices = [top_centre, bottom_centre]
vertices2 = [top_centre, bottom_centre]
faces = []
faces2 = []
for side in range(0, sides):
  # calculate x and z for start and end of each segment
  x1 = math.sin(2 * math.pi * side / sides)
  z1 = math.cos(2 * math.pi * side / sides)
  x2 = math.sin(2 * math.pi * (side + 1) / sides)
  z2 = math.cos(2 * math.pi * (side + 1) / sides)
  face_vertices = [(x1, 1, z1), (x1, -1, z1), (x2, 1, z2), (x2, -1, z2)]
  for vertex in face_vertices:
    if not vertex in vertex_map:
      vertices.append(vertex)
      vertices2.append(vertex)
      vertex_map[vertex] = len(vertices) - 1
  # cylinder side
  faces.append([
    vertex_map[face_vertices[0]],
    vertex_map[face_vertices[1]],
    vertex_map[face_vertices[3]],
    vertex_map[face_vertices[2]]])
  faces.append([
    vertex_map[face_vertices[0]],
    vertex_map[face_vertices[2]],
    vertex_map[face_vertices[2]],
    vertex_map[face_vertices[0]]])
  faces.append([
    vertex_map[face_vertices[1]],
    vertex_map[face_vertices[3]],
    vertex_map[face_vertices[3]],
    vertex_map[face_vertices[1]]])
  # top face
  faces2.append([
    vertex_map[face_vertices[0]],
    vertex_map[face_vertices[2]],
    vertex_map[vertices[0]],
    vertex_map[vertices[0]]])
  #faces2.append([
  #  vertex_map[vertices[0]],
  #  vertex_map[vertices[0]],
  #  vertex_map[face_vertices[2]],
  #  vertex_map[face_vertices[0]]])
  # bottom face
  faces2.append([
    vertex_map[vertices[1]],
    vertex_map[vertices[1]],
    vertex_map[face_vertices[3]],
    vertex_map[face_vertices[1]]])
  #faces2.append([
  #  vertex_map[face_vertices[1]],
  #  vertex_map[face_vertices[3]],
  #  vertex_map[vertices[1]],
  #  vertex_map[vertices[1]]])

f.write("cyl = gr.node('cyl')\n")

# write
f.write("cyl_body = gr.mesh('cyl_body', {\n")
# write vertices
f.write(",\n".join(
  [("  {" + (", ".join(map(str, vertex))) + "}")
    for vertex in vertices]
))
f.write("}, {\n")
# write faces
f.write(",\n".join(
  [("  {" + (", ".join(map(str, face))) + "}")
    for face in faces]
))
f.write("})\n")
if not final: f.write("cyl_body:set_material(test_mat)\n")
f.write("cyl:add_child(cyl_body)\n")

# write
f.write("cyl_ends = gr.mesh('cyl_ends', {\n")
# write vertices
f.write(",\n".join(
  [("  {" + (", ".join(map(str, vertex))) + "}")
    for vertex in vertices]
))
f.write("}, {\n")
# write faces
f.write(",\n".join(
  [("  {" + (", ".join(map(str, face))) + "}")
    for face in faces2]
))
f.write("})\n")
f.write("cyl_ends:no_outline()\n")
if not final: f.write("cyl_ends:set_material(test_mat)\n")
f.write("cyl:add_child(cyl_ends)\n")

if not final:
  f.write("rootnode:add_child(cyl)\n")
  f.write("rootnode:translate(0, 0, -10.0)\n")
  f.write("rootnode:rotate('x', -30)\n")
  f.write("return rootnode\n")
