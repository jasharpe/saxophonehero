table_brown = gr.textured_material({1, 1, 1}, {0.1, 0.1, 0.1}, 10, "data/wood.bmp", 1)

table = gr.node('table')

table_width = 1.5
table_height = 1
table_depth = 1.5

table_top_height = 0.2
dofile('data/cube.lua')
table_top = cube
table_top:set_material(table_brown)
table_top:translate(0, table_height, 0)
table_top:scale(table_width / 2, table_top_height / 2, table_depth / 2)
table:add_child(table_top)

table_side_offset = table_width / 2
table_depth_offset = table_depth / 2
for key,value in pairs({table_side_offset, -table_side_offset}) do
  sign = value / table_side_offset
  for key2,value2 in pairs({table_depth_offset, -table_depth_offset}) do
    sign2 = value2 / table_depth_offset
    leg_height = table_height
    leg_width = 0.1
    
    dofile('data/cyl.lua')
    leg = cyl
    cyl_ends:set_material(table_brown)
    cyl_body:set_material(table_brown)
    leg:translate(value - sign * 0.1, leg_height / 2, value2 - sign2 * 0.1)
    leg:scale(leg_width / 2, leg_height / 2, leg_width / 2)
    table:add_child(leg)
  end
end

--return table
