#!/usr/bin/python

import csv

##
# Automatically generates pianists hand motions based on .notes file
##

filename = "giantsteps.notes"
out_filenames = ["pianist_anim_left.lua", "pianist_anim_right.lua"]
head_out_filename = "pianist_anim_head.lua"

head_back = 10
head_forward = -10

rest_position_delta = -15
positionss = [[(-35, 50), (-28, 23), (-29, 6), (-28, -2), (-32, -16)], [(-33, 46), (-31, 40), (-29, 26), (-29, 11), (-33, -15)]]

# first read note file
r = csv.reader(open(filename, 'r'), delimiter=',')
max_time = 0
notes = []
for i, row in enumerate(r):
  if i == 0:
    max_time = int(row[0])
  else:
    notes.append((int(row[0]), int(row[1])))

# add the beginning note at the end as a fake
notes.append((notes[0][0] + max_time, notes[0][1]))

def print_anim(f, start, end, init_x, final_x, init_y, final_y, joint="shoulder_joint"):
    f.write("%s:add_joint_animation(%d, %d, {%d, %d}, {%d, %d})\n" % (joint, start, end, init_x, final_x, init_y, final_y))

for i in range(0, 2):
  out_filename = out_filenames[i]
  positions = positionss[i]

  f = open(out_filename, 'w')
  h = open(head_out_filename, 'w')
  prev = None
  head_counter = 0
  for i, (time, note) in enumerate(notes):
    # do this so the first note joins smoothly with the last
    if prev == None:
      prev = (notes[-2][0] - max_time, notes[-2][1])

    (prev_time, prev_note) = prev
    (prev_position_x, prev_position_y) = positions[prev_note - 1]
    (position_x, position_y) = positions[note - 1]

    # there are 3 animations associated with each pair of notes
    # up from prev to rest, from rest to rest, and from rest to
    # current. So time per is:
    time_per = (time - prev_time) / 3
    time_per_head = (time - prev_time) / 2
    
    factor = 1
    if (abs(note - prev_note) > 1):
      factor = 1.2

    # prev to rest
    print_anim(f,
      prev_time, prev_time + time_per / factor,
      prev_position_x, prev_position_x + rest_position_delta,
      prev_position_y, prev_position_y)

    # rest to rest
    print_anim(f,
      prev_time + time_per / factor, time - time_per / factor,
      prev_position_x + rest_position_delta, position_x + rest_position_delta,
      prev_position_y, position_y)

    # rest to note
    print_anim(f,
      time - time_per / factor, time,
      position_x + rest_position_delta, position_x,
      position_y, position_y)

    # for the head, should be back halfway between notes, and forward on
    # notes. This mean two phases. From prev to half way, front -> back,
    # from half way to note, back -> front
    if head_counter % 2 == 0:
      print_anim(h,
        prev_time, time,
        0, 0,
        head_forward, head_back, "head_joint")
    else:
      print_anim(h,
        prev_time, time,
        0, 0,
        head_back, head_forward, "head_joint")
    head_counter += 1
    

    prev = (time, note)
