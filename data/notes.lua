root = gr.node('root')

note = gr.sphere('note')
note:scale(1, 0.5, 1)
root:add_child(note)

return root
