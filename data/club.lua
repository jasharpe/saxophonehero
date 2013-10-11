red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.3, 0.3, 0.3}, {0.1, 0.1, 0.1}, 10)
grey = gr.material({0.6, 0.6, 0.6}, {0.1, 0.1, 0.1}, 10)
skin = gr.material({0.99609375, 0.87109375, 0.72265625}, {0.1, 0.1, 0.1}, 10)
gold = gr.material({0.99609375, 0.796875, 0.19921875}, {0.1, 0.1, 0.1}, 10)
wood_tile = gr.textured_material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10, "data/wood_tile.bmp", 0)
wood = gr.textured_material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10, "data/wood.bmp", 0)
wood1 = gr.textured_material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10, "data/wood.bmp", 1)
brick = gr.textured_material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10, "data/brick.bmp", 0)
drink_mat = gr.textured_material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10, "data/drinks.bmp", 2)

rootnode = gr.node('root')

-- ROOM
room_width = 40
room_height = 20
room_length = 80

room = gr.node('room')
room:translate(0, -4.5, 0)
rootnode:add_child(room)

-- FLOOR
plane = gr.mesh('plane', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1,  0, 1 }, 
		   { -1, 0, 1  },
       { -1, -0.01, -1 }, 
		   {  1, -0.01, -1 }, 
		   {  1, -0.01, 1 }, 
		   { -1, -0.01, 1  }
		}, {
		   {3, 2, 1, 0},
       {4, 5, 6, 7}
		})
room:add_child(plane)
plane:set_material(wood_tile)
plane:scale(room_width / 2, 1, room_length / 2)

-- WALLS
wall_node_major = gr.node('wall')
wall_node_major:translate(-20, 0, -room_length / 2)
wall_node_major:rotate('z', 90)
wall_node_major:scale(room_height / 2, 1, 1)
room:add_child(wall_node_major)

wall = gr.mesh('plane', {
       { -1, 0, -1 }, 
       {  1, 0, -1 }, 
       {  1,  0, 1 }, 
       { -1, 0, 1  },
       { -1, -0.01, -1 }, 
       {  1, -0.01, -1 }, 
       {  1, -0.01, 1 }, 
       { -1, -0.01, 1  }
    }, {
      -- {3, 2, 1, 0}
      {4, 5, 6, 7}
    })
wall_node_major:add_child(wall)
wall:set_material(brick)
wall:no_outline()
wall:translate(1, 0, room_length / 2)
wall:scale(1, 1, room_length / 2)

wall = gr.mesh('plane', {
       { -1, 0, -1 }, 
       {  1, 0, -1 }, 
       {  1,  0, 1 }, 
       { -1, 0, 1  },
       { -1, -0.01, -1 }, 
       {  1, -0.01, -1 }, 
       {  1, -0.01, 1 }, 
       { -1, -0.01, 1  }
    }, {
      -- {3, 2, 1, 0}
      {4, 5, 6, 7}
    })
wall_node_major:add_child(wall)
wall:set_material(brick)
wall:no_outline()
wall:translate(1, -room_width / 2, 0)
wall:rotate('x', -90)
wall:scale(1, 1, room_width / 2)

wall = gr.mesh('plane', {
       { -1, 0, -1 }, 
       {  1, 0, -1 }, 
       {  1,  0, 1 }, 
       { -1, 0, 1  },
       { -1, -0.01, -1 }, 
       {  1, -0.01, -1 }, 
       {  1, -0.01, 1 }, 
       { -1, -0.01, 1  }
    }, {
      -- {3, 2, 1, 0}
      {4, 5, 6, 7}
    })
wall_node_major:add_child(wall)
wall:set_material(brick)
wall:no_outline()
wall:translate(1, -room_width / 2, room_length)
wall:rotate('x', 90)
wall:scale(1, 1, room_width / 2)

wall = gr.mesh('plane', {
       { -1, 0, -1 }, 
       {  1, 0, -1 }, 
       {  1,  0, 1 }, 
       { -1, 0, 1  },
       { -1, -0.01, -1 }, 
       {  1, -0.01, -1 }, 
       {  1, -0.01, 1 }, 
       { -1, -0.01, 1  }
    }, {
      -- {3, 2, 1, 0}
      {4, 5, 6, 7}
    })
wall_node_major:add_child(wall)
wall:set_material(drink_mat)
wall:no_outline()
wall:translate(1, -room_width / 4, room_length - 0.03)
wall:rotate('x', 90)
wall:scale(0.5, 1, 0.7 * room_width / 4)

wall = gr.mesh('plane', {
       { -1, 0, -1 }, 
       {  1, 0, -1 }, 
       {  1,  0, 1 }, 
       { -1, 0, 1  },
       { -1, -0.01, -1 }, 
       {  1, -0.01, -1 }, 
       {  1, -0.01, 1 }, 
       { -1, -0.01, 1  }
    }, {
      -- {3, 2, 1, 0}
      {4, 5, 6, 7}
    })
wall_node_major:add_child(wall)
wall:set_material(wood)
wall:no_outline()
wall:translate(1, -room_width / 4, room_length - 0.015)
wall:rotate('x', 90)
wall:scale(0.6, 1, 0.8 * room_width / 4)

wall_node_major = gr.node('wall')
wall_node_major:translate(20, 0, -room_length / 2)
wall_node_major:rotate('z', 90)
wall_node_major:scale(room_height / 2, 1, 1)
room:add_child(wall_node_major)

wall_away = 1
for i=0,5 do
  wall_node = gr.node('wall_node')
  wall_node:translate(0, 0, i * 13.8)
  wall_node_major:add_child(wall_node)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.01, -1 }, 
         {  1, -0.01, -1 }, 
         {  1, -0.01, 1 }, 
         { -1, -0.01, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(1, wall_away, 1)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.01, -1 }, 
         {  1, -0.01, -1 }, 
         {  1, -0.01, 1 }, 
         { -1, -0.01, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(1, wall_away, 10)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.01, -1 }, 
         {  1, -0.01, -1 }, 
         {  1, -0.01, 1 }, 
         { -1, -0.01, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(2 - 0.2, wall_away, 5.5)
  wall:scale(0.2, 1, 3.5)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.01, -1 }, 
         {  1, -0.01, -1 }, 
         {  1, -0.01, 1 }, 
         { -1, -0.01, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(0.2, wall_away, 5.5)
  wall:scale(0.2, 1, 3.5)
  --wall:translate(20 - wall_away, room_height / 2, room_length / 2 - room_length / 30)
  --wall:scale(room_height / 2, 1, room_length / 30)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.001, -1 }, 
         {  1, -0.001, -1 }, 
         {  1, -0.001, 1 }, 
         { -1, -0.001, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(0.2 * 2, wall_away / 2, 5.5)
  wall:rotate('z', 90)
  wall:scale(wall_away / 2, 1, 3.5)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.001, -1 }, 
         {  1, -0.001, -1 }, 
         {  1, -0.001, 1 }, 
         { -1, -0.001, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(2 - 0.2 * 2, wall_away / 2, 5.5)
  wall:rotate('z', 90)
  wall:scale(wall_away / 2, 1, 3.5)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.01, -1 }, 
         {  1, -0.01, -1 }, 
         {  1, -0.01, 1 }, 
         { -1, -0.01, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(1, wall_away / 2, 2)
  wall:rotate('x', 90)
  wall:scale(0.6, 1, wall_away / 2)

  wall = gr.mesh('plane', {
         { -1, 0, -1 }, 
         {  1, 0, -1 }, 
         {  1,  0, 1 }, 
         { -1, 0, 1  },
         { -1, -0.01, -1 }, 
         {  1, -0.01, -1 }, 
         {  1, -0.01, 1 }, 
         { -1, -0.01, 1  }
      }, {
         {3, 2, 1, 0},
         {4, 5, 6, 7}
      })
  wall_node:add_child(wall)
  wall:set_material(brick)
  wall:no_outline()
  wall:translate(1, wall_away / 2, 9)
  wall:rotate('x', 90)
  wall:scale(0.6, 1, wall_away / 2)

  if (i < 5) then
    wall = gr.mesh('plane', {
           { -1, 0, -1 }, 
           {  1, 0, -1 }, 
           {  1,  0, 1 }, 
           { -1, 0, 1  },
           { -1, -0.01, -1 }, 
           {  1, -0.01, -1 }, 
           {  1, -0.01, 1 }, 
           { -1, -0.01, 1  }
        }, {
           {3, 2, 1, 0},
           {4, 5, 6, 7}
        })
    wall_node:add_child(wall)
    wall:set_material(brick)
    wall:no_outline()
    gap_width = 2.8
    wall:translate(1, wall_away, 12 + gap_width / 2 - 1)
    wall:scale(1, 1, gap_width / 2)
  end
end

puppet_x = 0
puppet_z = -15
require('data/puppet')
rootnode:add_child(puppet)

require('data/pianist')
pianist_full:translate(-9, -0.45, -27)
pianist_full:rotate('y', 30)
rootnode:add_child(pianist_full)

require('data/smoker')
smoker:translate(0, 0, 32.5)
smoker:rotate('x', -15)
smoker:rotate('y', 180)
rootnode:add_child(smoker)

dofile('data/table.lua')
table:translate(8, -4.48, 5)
table:rotate('y', -15)
table:scale(3, 3, 3)
rootnode:add_child(table)

chair_scale = 2

dofile('data/chair.lua')
chair:translate(8, -4.48, 10)
chair:rotate('y', 10)
chair:scale(chair_scale, chair_scale, chair_scale)
rootnode:add_child(chair)

dofile('data/chair.lua')
chair:translate(12, -4.48, 7.5)
chair:rotate('y', 50)
chair:scale(chair_scale, chair_scale, chair_scale)
rootnode:add_child(chair)

dofile('data/table.lua')
table:translate(-8, -4.48, 7)
table:rotate('y', 40)
table:scale(6, 3, 3)
rootnode:add_child(table)

dofile('data/chair.lua')
chair:translate(-6, -4.48, 11)
chair:rotate('y', 10)
chair:scale(chair_scale, chair_scale, chair_scale)
rootnode:add_child(chair)

dofile('data/table.lua')
table:translate(3, -4.48, 16.5)
table:rotate('y', 30)
table:scale(3, 3, 3)
rootnode:add_child(table)

dofile('data/chair.lua')
chair:translate(5, -4.48, 21)
chair:rotate('y', -20)
chair:scale(chair_scale, chair_scale, chair_scale)
rootnode:add_child(chair)

-- BAR
dofile('data/cube.lua')
bar = cube
bar:set_material(wood1)
bar_height = 4
bar_width = 25
bar_depth = 4
bar:translate(-7.25, -4.5 + bar_height / 2 + 0.2, 30)
bar:scale(bar_width / 2, bar_height / 2, bar_depth / 2)
rootnode:add_child(bar)

rootnode:translate(0, 0, -10)
rootnode:scale(0.5, 0.5, 0.5)
return rootnode
