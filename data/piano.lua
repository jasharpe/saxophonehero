piano_white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
piano_black = gr.material({0.2, 0.2, 0.2}, {0.1, 0.1, 0.1}, 10)
piano_brown = gr.material({133 / 255, 94 / 255, 66 / 255}, {0.1, 0.1, 0.1}, 10)
piano_gold = gr.material({0.99609375, 0.796875, 0.19921875}, {0.1, 0.1, 0.1}, 10)

piano = gr.node('piano')

inst = gr.node('inst')
piano:add_child(inst)

piano_width = 6
piano_height = 4
piano_depth = 1.5

dofile('data/cube.lua')
body = cube
body:set_material(piano_brown)
body:scale(piano_width / 2, piano_height / 2, piano_depth / 2)
inst:add_child(body)

piano_side_offset = piano_width / 2
side_width = 0.2
side_height = 0.9
side_depth = 1
side_inset = 0.1
bottom_side_height = 0.3
for key,value in pairs({piano_side_offset, -piano_side_offset}) do
  sign = (value / piano_side_offset)

  dofile('data/cube.lua')
  side = cube
  side:set_material(piano_brown)
  side:translate(value - sign * side_width / 2 - sign * side_inset, 0, piano_depth / 2 + side_depth / 2)
  side:scale(side_width / 2, side_height / 2, side_depth / 2)
  inst:add_child(side)  

  dofile('data/cube.lua')
  bottom_side = cube
  bottom_side:set_material(piano_brown)
  bottom_side:translate(
    value - sign * side_width / 2 - sign * side_inset,
    -piano_height / 2 + bottom_side_height / 2,
    piano_depth / 2 + side_depth / 2)
  bottom_side:scale(side_width / 2, bottom_side_height / 2, side_depth / 2)
  inst:add_child(bottom_side)

  dofile('data/cyl.lua')
  post = cyl
  cyl_ends:set_material(piano_brown)
  cyl_body:set_material(piano_brown)
  post:translate(value - sign * side_width / 2 - sign * side_inset, -0.8, piano_depth / 2 + side_depth * 3 / 4)
  post:scale(0.8 * side_width / 2, 1, 0.8 * side_width / 2)
  inst:add_child(post)
end

piano_pedal_offset = piano_width / 20
pedal_height = bottom_side_height / 4
pedal_depth = side_depth * (1 / 2)
pedal_width = side_width * 3 / 4
for key,value in pairs({piano_pedal_offset, 0, -piano_pedal_offset}) do
  dofile('data/cube.lua')
  pedal = cube
  pedal:set_material(piano_brown)
  pedal:translate(
    value - sign * side_width / 2 - sign * side_inset,
    -piano_height / 2 + pedal_height / 2 + 0.05,
    piano_depth / 2 + pedal_depth / 2)
  pedal:scale(pedal_width / 2, pedal_height / 2, pedal_depth / 2)
  inst:add_child(pedal)
end

keyboard_base_width = piano_width - 2 * side_width - 2 * side_inset
keyboard_base_height = 0.3
keyboard_base_depth = 0.9 * side_depth
keyboard_base_height_offset = 0.05

dofile('data/cube.lua')
keyboard_base = cube
keyboard_base:set_material(piano_brown)
keyboard_base:translate(0,
  -side_height / 2 + keyboard_base_height / 2 + keyboard_base_height_offset,
  piano_depth / 2 + keyboard_base_depth / 2)
keyboard_base:scale(keyboard_base_width / 2, keyboard_base_height / 2, keyboard_base_depth / 2)
inst:add_child(keyboard_base)

keyboard_width = keyboard_base_width
keyboard_height = 0.2
keyboard_depth = 0.9 * keyboard_base_depth
keyboard_height_offset = keyboard_base_height_offset + keyboard_base_height / 2

keyboard_back_height_offset = keyboard_height_offset + 0.02 + 0.1
keyboard_back_depth_offset = 5 * keyboard_base_depth / 16

keyboard_back_height = 1
back_extra = 0.2
dofile('data/cube.lua')
keyboard_back = cube
keyboard_back:set_material(piano_brown)
keyboard_back:translate(0,
  -side_height / 2 + keyboard_back_height_offset + back_extra / 2,
  piano_depth / 2 - keyboard_back_depth_offset)
keyboard_back:scale(keyboard_base_width / 2, (keyboard_back_height - back_extra) / 2, keyboard_base_depth / 2)
inst:add_child(keyboard_back)

music_stand_height = 1
music_stand_width = piano_width / 2
music_stand_depth = 0.01
dofile('data/cube.lua')
music_stand = cube
music_stand:set_material(piano_brown)
music_stand:translate(0,
  music_stand_height / 2 + keyboard_back_height_offset + 0.06,
  piano_depth / 2 + keyboard_back_depth_offset / 2 - 0.075)
music_stand:rotate('x', -7)
music_stand:scale(music_stand_width / 2, music_stand_height / 2, music_stand_depth / 2)
inst:add_child(music_stand)

number_of_keys = 30
-- 0, 1, 3, 4, and 5 (mod 7) get black keys
for i=0,number_of_keys do
  dofile('data/simple_cube.lua')
  keyboard = cube
  imod7 = (i - 1) % 7
  black_key = ((imod7 == 0 or imod7 == 1 or imod7 == 3 or imod7 == 4 or imod7 == 5) and (i ~= number_of_keys - 1))
  if (i == 0 or i == 30) then
    black_key = false
    keyboard:set_material(piano_brown)
  else
    keyboard:set_material(piano_white)
  end
  keyboard:translate(keyboard_width * ((i - number_of_keys / 2) / number_of_keys),
    -side_height / 2 + keyboard_height / 2 + keyboard_height_offset,
    piano_depth / 2 + keyboard_depth / 2)
  keyboard:scale(keyboard_width / 2 / number_of_keys - 0.001, keyboard_height / 2, keyboard_depth / 2)
  inst:add_child(keyboard)

  black_key_height_offset = 0.05
  if (black_key) then
    dofile('data/simple_cube.lua')
    black_keyboard = cube
    black_keyboard:set_material(piano_black)
    black_keyboard:translate(
      keyboard_width * ((i - number_of_keys / 2 + 0.5) / number_of_keys),
      -side_height / 2 + keyboard_height / 2 + keyboard_height_offset + black_key_height_offset,
      piano_depth / 2 + keyboard_depth / 2 / 2)
    black_keyboard:scale(keyboard_width / 2 / number_of_keys / 2, keyboard_height / 2, keyboard_depth / 2)
    inst:add_child(black_keyboard)
  end
end

bench_width = piano_width * (4 / 9)
bench_height = piano_height * (3 / 9)
bench_depth = piano_depth * 0.8

bench = gr.node('bench')
bench:translate(0, -piano_height / 2, 3)
piano:add_child(bench)

bench_top_height = 0.2
dofile('data/cube.lua')
bench_top = cube
bench_top:set_material(piano_brown)
bench_top:translate(0, bench_height, 0)
bench_top:scale(bench_width / 2, bench_top_height / 2, bench_depth / 2)
bench:add_child(bench_top)

bench_side_offset = bench_width / 2
bench_depth_offset = bench_depth / 2
for key,value in pairs({bench_side_offset, -bench_side_offset}) do
  sign = value / bench_side_offset
  for key2,value2 in pairs({bench_depth_offset, -bench_depth_offset}) do
    sign2 = value2 / bench_depth_offset
    leg_height = bench_height
    leg_width = 0.1
    
    dofile('data/cyl.lua')
    leg = cyl
    cyl_ends:set_material(piano_brown)
    cyl_body:set_material(piano_brown)
    leg:translate(value - sign * 0.1, leg_height / 2, value2 - sign2 * 0.1)
    leg:scale(leg_width / 2, leg_height / 2, leg_width / 2)
    bench:add_child(leg)
  end
end
