white = gr.material({0.9, 0.9, 0.9}, {0.1, 0.1, 0.1}, 10)

root = gr.node('root')

top = gr.sphere('top')
top:translate(0, 0, 0.25)
top:rotate('z', 90)
top:scale(0.5, 0.5, 0.4)
--top:set_material(white)
root:add_child(top)

return root
