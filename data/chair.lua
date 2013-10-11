chair_brown = gr.textured_material({1, 1, 1}, {0.1, 0.1, 0.1}, 10, "data/wood.bmp", 1)

chair = gr.node('chair')

chair_width = 1
chair_height = 0.75
chair_depth = 1

chair_top_height = 0.2
dofile('data/cube.lua')
chair_top = cube
chair_top:set_material(chair_brown)
chair_top:translate(0, chair_height, 0)
chair_top:scale(chair_width / 2, chair_top_height / 2, chair_depth / 2)
chair:add_child(chair_top)

chair_back_width = 1
chair_back_height = 1.2
chair_back_depth = 0.2

dofile('data/cube.lua')
chair_back = cube
chair_back:set_material(chair_brown)
chair_back:translate(0, chair_height + chair_top_height / 2 + chair_back_height / 2, chair_depth / 2)
chair_back:rotate('x', 10)
chair_back:scale(chair_back_width / 2, chair_back_height / 2, chair_back_depth / 2)
chair:add_child(chair_back)

chair_side_offset = chair_width / 2
chair_depth_offset = chair_depth / 2
for key,value in pairs({chair_side_offset, -chair_side_offset}) do
  sign = value / chair_side_offset
  for key2,value2 in pairs({chair_depth_offset, -chair_depth_offset}) do
    sign2 = value2 / chair_depth_offset
    leg_height = chair_height
    leg_width = 0.1
    
    dofile('data/cyl.lua')
    leg = cyl
    cyl_ends:set_material(chair_brown)
    cyl_body:set_material(chair_brown)
    leg:translate(value - sign * 0.1, leg_height / 2, value2 - sign2 * 0.1)
    leg:scale(leg_width / 2, leg_height / 2, leg_width / 2)
    chair:add_child(leg)
  end
end

return chair
