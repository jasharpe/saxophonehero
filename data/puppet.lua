puppet = gr.joint('puppet', {-180, 0, 180}, {-180, 0, 180})

--anim_first_half = 25950 / 2
--anim_second_half = 2 * anim_first_half
anim_first_half = 25950 / 64
anim_second_half = 2 * anim_first_half

delta_z = -17.7664 + 17.8338
delta_y = -2 * (-1.74543 + 2.10049)
puppet:add_animation(0, anim_first_half, {puppet_x, 0, puppet_z}, {puppet_x, delta_y, puppet_z + delta_z})
puppet:add_animation(anim_first_half, anim_second_half, {puppet_x, delta_y, puppet_z + delta_z}, {puppet_x, 0, puppet_z})
puppet:add_joint_animation(0, anim_first_half, {0, 12}, {0, 0})
puppet:add_joint_animation(anim_first_half, anim_second_half, {12, 0}, {0, 0})
puppet:anim_slowable()

torso_height = 3
torso = gr.sphere('torso')
torso:scale(1, torso_height / 2, 0.5)
torso:set_material(white)
puppet:add_child(torso)

shoulders_height = 1
shoulders_width = 3.8
shoulders = gr.sphere('shoulders')
shoulders:translate(0, torso_height / 2, 0)
shoulders:scale(shoulders_width / 2, shoulders_height / 2, 0.5)
shoulders:set_material(white)
puppet:add_child(shoulders)

hips_node = gr.node('hips_node')
hips_node:translate(0, -torso_height / 2 + 0.4, 0)
puppet:add_child(hips_node)

hips_width = 1.8
hips_height = 1.2
hips = gr.sphere('hips')
hips:scale(hips_width / 2, hips_height / 2, 0.5)
hips:set_material(black)
hips_node:add_child(hips)

belt_height = hips_height / 2 - 0.3
belt = gr.sphere('belt')
belt:translate(0, belt_height, 0)
belt:scale(1, 0.2, 0.5)
belt:set_material(grey)
hips_node:add_child(belt)

belt_buckle = gr.sphere('belt_buckle')
belt_buckle:translate(0, belt_height, 0.5)
belt_buckle:scale(0.15, 0.1, 0.05)
belt_buckle:set_material(gold)
hips_node:add_child(belt_buckle)

-- ARMS
shoulders_apart = shoulders_width / 2 - 0.2
for key,value in pairs({shoulders_apart, -shoulders_apart}) do
  if key == 2 then
    shoulder_y = {-50, 35, 50}
    shoulder_x = {-180, -61, 45}
  else
    shoulder_y = {-50, -42, 50}
    shoulder_x = {-180, -30, 45}
  end
  shoulder_joint = gr.joint('shoulder_joint', shoulder_x, shoulder_y)
  shoulder_joint:translate(value, torso_height / 2, 0)
  puppet:add_child(shoulder_joint)

  -- height is 1.6, amount above joint is then 0.1
  upper_arm = gr.sphere('upper_arm')
  upper_arm:translate(0, -0.7, 0)
  upper_arm:scale(0.35, 0.8, 0.35)
  upper_arm:set_material(white)
  shoulder_joint:add_child(upper_arm)

  elbow_joint = gr.joint('elbow_joint', {-150, -45, 0}, {0, 0, 0})
  elbow_joint:translate(0, -1.4, 0)
  shoulder_joint:add_child(elbow_joint)

  fore_arm = gr.sphere('lower_arm')
  fore_arm:translate(0, -0.6, 0)
  fore_arm:scale(0.25, 0.7, 0.25)
  fore_arm:set_material(white)
  elbow_joint:add_child(fore_arm)

  if key == 2 then
    wrist_y = {90, 90, 90}
    wrist_x = {-90, -30, 90}
  else
    wrist_y = {90, 90, 90}
    wrist_x = {-90, 30, 90}
  end
  wrist_joint = gr.joint('wrist_joint', wrist_x, wrist_y)
  wrist_joint:translate(0, -1.2, 0)
  elbow_joint:add_child(wrist_joint)

  hand = gr.sphere('hand')
  hand:translate(0, -0.2, 0)
  hand:scale(0.25, 0.3, 0.15)
  hand:set_material(skin)
  wrist_joint:add_child(hand)
end

reverse = function(l)
  return {l[1], l[0]}
end

-- LEGS
hip_offset = hips_width / 2 - 0.3
for key,value in pairs({hip_offset, -hip_offset}) do
  -- RIGHT
  hip_joint_x = {-120, -28, 50}
  hip_joint_x_anim = {-28, -65}
  hip_joint_x_anim_back = {-65, -28}
  knee_joint_x = {0, 41, 130}
  knee_joint_x_anim = {41, 90}
  knee_joint_x_anim_back = {90, 41}
  ankle_joint_x = {-50, -15, 60}
  ankle_joint_x_anim = {-15, -35}
  ankle_joint_x_anim_back = {-35, -15}
  -- LEFT
  if key == 1 then
    hip_joint_x = {-120, 8, 50}
    hip_joint_x_anim = {8, -36}
    hip_joint_x_anim_back = {-36, 8}
    knee_joint_x = {0, 28, 130}
    knee_joint_x_anim = {28, 96}
    knee_joint_x_anim_back = {96, 28}
    ankle_joint_x = {-50, -38, 60}
    ankle_joint_x_anim = {-38, -50}
    ankle_joint_x_anim_back = {-50, -38}
  end
  hip_joint = gr.joint('hip_joint', hip_joint_x, {0, 0, 0})
  hip_joint:add_joint_animation(0, anim_first_half, hip_joint_x_anim, {0, 0})
  hip_joint:add_joint_animation(anim_first_half, anim_second_half, hip_joint_x_anim_back, {0, 0})
  hip_joint:anim_slowable()
  hip_joint:translate(value, 0, 0)
  hips_node:add_child(hip_joint)

  upper_leg_length = 1.8
  upper_leg_width = 0.5
  upper_leg = gr.sphere('upper_leg')
  upper_leg:translate(0, -upper_leg_length / 2, 0)
  upper_leg:scale(upper_leg_width, upper_leg_length / 2 + 0.2, upper_leg_width)
  upper_leg:set_material(black)
  hip_joint:add_child(upper_leg)

  knee_joint = gr.joint('knee_joint', knee_joint_x, {0, 0, 0})
  knee_joint:add_joint_animation(0, anim_first_half, knee_joint_x_anim, {0, 0})
  knee_joint:add_joint_animation(anim_first_half, anim_second_half, knee_joint_x_anim_back, {0, 0})
  knee_joint:anim_slowable()
  knee_joint:translate(0, -upper_leg_length, 0)
  hip_joint:add_child(knee_joint)

  fore_leg_length = 1.4
  fore_leg_width = 0.4
  fore_leg = gr.sphere('lower_leg')
  fore_leg:translate(0, -fore_leg_length / 2, 0)
  fore_leg:scale(fore_leg_width, fore_leg_length / 2 + 0.2, fore_leg_width)
  fore_leg:set_material(black)
  knee_joint:add_child(fore_leg)

  ankle_joint = gr.joint('ankle_joint', ankle_joint_x, {0, 0, 0})
  ankle_joint:add_joint_animation(0, anim_first_half, ankle_joint_x_anim, {0, 0})
  ankle_joint:add_joint_animation(anim_first_half, anim_second_half, ankle_joint_x_anim_back, {0, 0})
  ankle_joint:anim_slowable()
  ankle_joint:translate(0, -1.4, 0)
  knee_joint:add_child(ankle_joint)

  foot = gr.sphere('foot')
  foot:translate(0, -0.2, 0.4)
  foot:scale(0.25, 0.3, 0.5)
  foot:set_material(black)
  ankle_joint:add_child(foot)
end

-- NECK AND HEAD

neck_joint = gr.joint('neck_joint', {-40, 0, 50}, {0, 0, 0})
neck_joint:translate(0, torso_height / 2 + shoulders_height / 2 - 0.1, 0)
puppet:add_child(neck_joint)

neck_height = 0.8
neck_width = 0.25
neck = gr.sphere('neck')
neck:translate(0, 0.3, 0)
neck:scale(neck_width, neck_height / 2, neck_width)
neck:set_material(skin)
neck_joint:add_child(neck)

-- this is how much it could theoretically move without clipping,
-- but to keep things simple, disallow head movements and make
-- all movement come from the hips
head_joint = gr.joint('head_joint', {-30, 0, 5}, {-70, 0, 70})
head_joint:translate(0, 0.6, 0)
neck_joint:add_child(head_joint)

head_radius = 0.5

head_centre = gr.node('head')
head_centre:translate(0, head_radius - 0.1, 0)
head_joint:add_child(head_centre)

head = gr.sphere('head')
head:translate(0, head_radius - 0.1, 0)
head:scale(head_radius, head_radius, head_radius)
head:set_material(skin)
head_joint:add_child(head)

nose = gr.sphere('nose')
nose:translate(0, 0, 0.38)
nose:scale(0.1, 0.1, 0.2)
nose:set_material(skin)
head_centre:add_child(nose)

-- EYES
eye_width = 0.25
for key,value in pairs({eye_width / 2, -eye_width / 2}) do
  eye_centre_x = value
  eye_centre_y = 0.17
  eye_centre_z = 0.37

  eye = gr.node('eye')
  eye:translate(eye_centre_x, eye_centre_y, eye_centre_z)
  eye:rotate('x', -20)
  eye_scale = 0.2
  eye:scale(eye_scale / 2, eye_scale, 0.1)
  
  head_centre:add_child(eye)
  
  eye_white = gr.sphere('eye_white')
  eye_white:set_material(white)
  eye_white:no_outline()
  eye:add_child(eye_white)
  
  pupil = gr.sphere('pupil')
  pupil_offset = 0.85
  --if key == 2 then
  --  pupil_offset = -pupil_offset
  --end
  pupil:translate(0, 0, pupil_offset)
  pupil_scale = 0.4
  pupil:scale(pupil_scale, pupil_scale, pupil_scale)
  pupil:set_material(black)
  pupil:no_outline()
  eye:add_child(pupil)
end

-- EARS
ears_apart = head_radius
for key,value in pairs({ears_apart, -ears_apart}) do
  ear = gr.sphere('ear')
  ear:translate(value, 0, 0)
  ear:scale(0.1, 0.1, 0.1)
  ear:set_material(skin)
  head_centre:add_child(ear)
end

hat = gr.sphere('hat')
hat:translate(0, 0.25, -0.15)
hat:rotate('x', -45)
hat:scale(0.6, 0.3, 0.6)
hat:set_material(black)
head_centre:add_child(hat)

pom = gr.sphere('pom')
pom:translate(0, 0.45, -0.35)
pom:scale(0.15, 0.15, 0.15)
pom:set_material(black)
head_centre:add_child(pom)

require('data/sax_final')

sax = gr.node('sax_node')
sax:add_child(sax0)
sax:add_child(sax1)
head_centre:add_child(sax)
sax:translate(0, -0.2, 0.5)
sax:rotate('x', 12)
sax:rotate('y', -90)
sax:scale(0.0125, 0.0125, 0.0125)

end_of_sax = gr.particle_emitter('end_of_sax', 1)
--end_of_sax:set_material(green)
end_of_sax:translate(0, -2.9, 2.3)
--end_of_sax:scale(0.1, 0.1, 0.1)
head_centre:add_child(end_of_sax)

end_of_sax2 = gr.particle_emitter('end_of_sax2', 2)
--end_of_sax:set_material(green)
end_of_sax2:translate(0, -2.9, 2.3)
--end_of_sax:scale(0.1, 0.1, 0.1)
head_centre:add_child(end_of_sax2)

puppet_height = 9
