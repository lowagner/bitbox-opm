#!/usr/bin/env python2
from math import *
core = [
    'head', 'chest', 'torso'
]

core_deltas = {
    'head' : 0.02,
    'chest' : 0.01,
    'torso' : 0,
}

animation = [
    "idle_R",
    "idle_L",
    "guard_R",
    "guard_L",
    "walk_R_0",
    "walk_L_0",
    "walk_R_1",
    "walk_L_1",
    "run_R_0",
    "run_L_0",
    "run_R_1",
    "run_L_1",
    "crouch_R", # pre and post jump (jump crouch and landing) 
    "crouch_L",
    "fly_R",
    "fly_L",
    "punch_R_0",
    "punch_L_0",
    "punch_R_1",
    "punch_L_1",
    "kick_R_0",
    "kick_L_0",
    "kick_R_1",
    "kick_L_1",
    "grab",
    "grab_throw",
    "grab_punch",
    "grab_kick",
    "user_R",
    "user_L",
    "hurt",
    "bad_hurt"
]

limbs = [
    "R_elbow",
    "R_wrist",
    "R_knuckle",
    "L_elbow",
    "L_wrist",
    "L_knuckle",
    "R_knee",
    "R_ankle",
    "R_toe",
    "L_knee",
    "L_ankle",
    "L_toe"
]
   
limb_deltas = {
    "R_elbow" : 0.06,
    "R_wrist" : 0.08,
    "R_knuckle" : 0.07,
    "L_elbow" : -0.04,
    "L_wrist" : -0.06,
    "L_knuckle": -0.05,
    "R_knee" : 0.03,
    "R_ankle" : 0.05,
    "R_toe" : 0.04,
    "L_knee" : -0.01,
    "L_ankle": -0.02,
    "L_toe" : -0.03
}

with open("src/switch_animation.h", 'w') as f:
    f.write('// autogenerated by mk_animation.py\n')
    i = 0
    for a in animation:
        f.write('#define ANIM_%s %d\n'%(a.upper(), i))
        i += 1
    f.write('#define ANIM_COUNT %d\n'%i)
    f.write('#define ANIM_FACE_LEFT 128\n')

    f.write('typedef struct animation_core\n{\n')
    for p in core:
        f.write('\tanimation_absolute %s;\n'%p)
    f.write('}\nanimation_core;\n')

    f.write('typedef struct animation_frame\n{\n')
    f.write('\tanimation_core core;\n')
    i = 0
    for p in limbs:
        f.write('\tanimation_limb %s;\n'%p)
        i += 1
    f.write('}\nanimation_frame;\n')
    f.write('#define PART_COUNT %d\n'%i)
    f.write('struct animation\n{\n')
    f.write('\tfloat tween;\n')
    f.write('\tuint8_t frames[3];\n')
    f.write('\tuint8_t mix_from_to;\n')
    f.write('\tanimation_core core[3];\n')
    for p in limbs:
        f.write('\tanimation_limb %s[3];\n'%p)
    f.write('};\n')

with open("src/switch_animation.c", 'w') as f:
    f.write('// autogenerated by mk_animation.py\n')
    
    f.write('\tconst float sine[256] = {\n')
    for i in range(256):
        f.write('\t%0.11f,\n'%sin(2*pi*i/256)) 
    f.write('\t};\n\n')

    f.write('void _animation_reset(int p)\n{\n')
    f.write('\tfor (int k=1; k<16; ++k)\n\t{\n')
    f.write('\t\tquads[32*p+k].x = players[p].x;\n')
    f.write('\t\tquads[32*p+k].y = players[p].y;\n')
    f.write('\t\tquads[32*p+k].z = players[p].z;\n')
    f.write('\t}\n')
    f.write('}\n\n')

    f.write('void _animation_copy_to(int p, int a_to, int next_frame)\n{\n')
    f.write('\tif (players[p].animation.frames[a_to] == next_frame)\n\t\treturn;\n')
    f.write('\tplayers[p].animation.frames[a_to] = next_frame;\n')
    f.write('\tconst struct animation_frame *frame = &players[p].all_frames[next_frame&(~ANIM_FACE_LEFT)];\n')
    f.write('\tif (!(next_frame&ANIM_FACE_LEFT))\n\t{\n')
    for p in core:
        f.write('\t\tplayers[p].animation.core[a_to].%s = frame->core.%s;\n'%(p,p))
    for p in limbs:
        f.write('\t\tplayers[p].animation.%s[a_to] = frame->%s;\n'%(p,p))
    f.write('\t}\n\telse\n\t{\n')
    f.write('\t\tanimation_absolute *a;\n')
    f.write('\t\tanimation_limb *l;\n')
    for p in core:
        f.write('\t\ta = &players[p].animation.core[a_to].%s;\n'%p)
        f.write('\t\t*a = frame->core.%s;\n'%p)
        f.write('\t\ta->dx = -a->dx - a->width;\n')
    for p in limbs:
        f.write('\t\tl = &players[p].animation.%s[a_to];\n'%p)
        f.write('\t\t*l = frame->%s;\n'%p)
        if p in set(['R_elbow', 'L_elbow', 'R_knee', 'L_knee']):
            f.write('\t\tl->angle = -128 - l->angle;\n')
        else:
            f.write('\t\tl->angle *= -1;\n')
    f.write('\t}\n}\n')

    f.write('void _animation_tween(int p, const float delta, const float rate, const int a_mix, const int a_from, const int a_to)\n{\n')
    f.write('\tconst float tween_to = players[p].animation.tween;\n')
    f.write('\tconst float tween_from = 1.0f - tween_to;\n')
    f.write('\tanimation_core *to_core = &players[p].animation.core[a_to];\n');
    f.write('\tanimation_core *mix_core = &players[p].animation.core[a_mix];\n');
    f.write('\tanimation_core *from_core = &players[p].animation.core[a_from];\n');
    for p in core:
      for q in ['dx', 'dy', 'width', 'height']:
        f.write('\tmix_core->%s.%s = round(tween_from*from_core->%s.%s + tween_to*to_core->%s.%s);\n'%(p,q,p,q,p,q))
    f.write('\tint8_t delta_angle;\n')
    for p in limbs:
        f.write('\tdelta_angle = players[p].animation.%s[a_to].angle - players[p].animation.%s[a_from].angle;\n'%(p,p))
        f.write('\tplayers[p].animation.%s[a_mix].angle = players[p].animation.%s[a_from].angle + (int)round(tween_to*delta_angle);\n'%(p,p))
        f.write('\tplayers[p].animation.%s[a_mix].length = round(tween_from*players[p].animation.%s[a_from].length + tween_to*players[p].animation.%s[a_to].length);\n'%(p,p,p))

    f.write('\tint k=32*p;\n')
    f.write('\tstruct {float x, y, z; int ix, iy;} new_pos;\n')
    for p in core:
        f.write('// %s\n'%p)
        f.write('\tquads[++k].ix = players[p].ix + mix_core->%s.dx;\n'%p)
        f.write('\tquads[k].iy = players[p].iy - mix_core->%s.dy;\n'%p)
        f.write('\tnew_pos.x = players[p].x + mix_core->%s.dx;\n'%p);
        f.write('\tnew_pos.y = players[p].y + %ff;\n'%core_deltas[p]);
        f.write('\tnew_pos.z = players[p].z - mix_core->%s.dy;\n'%p);
        f.write('\tquads[k].width = mix_core->%s.width;\n'%p)
        f.write('\tquads[k].height = mix_core->%s.height;\n'%p)
        f.write('\tquads[k].vx = (new_pos.x - quads[k].x)/delta + 0.5*rate*(to_core->%s.width - from_core->%s.width);\n'%(p,p))
        f.write('\tquads[k].vy = (new_pos.y - quads[k].y)/delta;\n')
        f.write('\tquads[k].vz = (new_pos.z - quads[k].z)/delta + 0.5*rate*(to_core->%s.height - from_core->%s.height);\n'%(p,p))
        f.write('\tquads[k].x = new_pos.x;\n')
        f.write('\tquads[k].y = new_pos.y;\n');
        f.write('\tquads[k].z = new_pos.z;\n');
   
    f.write('\tstruct {float x, y, z; int ix, iy;} old_pos;\n')
    f.write('\tuint8_t angle;\n')
    f.write('\tanimation_limb *limb;\n')
    for region in ['top', 'bottom']:
        for side in ['R', 'L']: 
            if region == 'top':
                if side == 'L':
                    f.write('\told_pos.x = players[p].x + mix_core->chest.dx + mix_core->chest.width + 1;\n')
                else:
                    f.write('\told_pos.x = players[p].x + mix_core->chest.dx - 1;\n')
                f.write('\told_pos.y = players[p].y;\n')
                f.write('\told_pos.z = players[p].z - mix_core->chest.dy;\n')

                subs = ['elbow', 'wrist', 'knuckle']
                f.write('\tlimb = &players[p].animation.%s_elbow[a_mix];\n'%side)
            else:
                if side == 'L':
                    f.write('\told_pos.x = players[p].x + mix_core->torso.dx + mix_core->torso.width - 1;\n')
                else:
                    f.write('\told_pos.x = players[p].x + mix_core->torso.dx + 1;\n')
                f.write('\told_pos.y = players[p].y;\n')
                f.write('\told_pos.z = players[p].z - mix_core->torso.dy + mix_core->torso.height + 1;\n')

                subs = ['knee', 'ankle', 'toe']
                f.write('\tlimb = &players[p].animation.%s_knee[a_mix];\n'%side)
            
            f.write('\told_pos.ix = round(old_pos.x - map_offset_x);\n')
            f.write('\told_pos.iy = STREET_LEVEL_Y + round(old_pos.y + old_pos.z);\n')
            
            for s in range(3):
                sub = subs[s]
                p = side+"_"+sub
                f.write('// %s\n'%p)
                if s == 0:
                    f.write('\tangle = limb->angle;\n')
                else:
                    f.write('\tangle += limb->angle;\n')
                f.write('\tnew_pos.x = old_pos.x + limb->length * sine[(angle+64)&255];\n');
                f.write('\tnew_pos.y = players[p].y + %ff;\n'%limb_deltas[p]);
                f.write('\tnew_pos.z = old_pos.z + limb->length * sine[angle];\n');
                f.write('\tnew_pos.ix = round(new_pos.x - map_offset_x);\n')
                f.write('\tnew_pos.iy = STREET_LEVEL_Y + round(new_pos.y + new_pos.z);\n')
                f.write('\t++k;\n');
                f.write('\tquads[k].vx = (new_pos.x - quads[k].x)/delta;\n')
                f.write('\tquads[k].vy = (new_pos.y - quads[k].y)/delta;\n')
                f.write('\tquads[k].vz = (new_pos.z - quads[k].z)/delta;\n')
                f.write('\tquads[k].x = new_pos.x;\n')
                f.write('\tquads[k].y = new_pos.y;\n');
                f.write('\tquads[k].z = new_pos.z;\n');
                # TODO!!! check
                if region == 'top':
                    min_length = "mix_core->torso.width/2 + %d"%((2-s)/2)
                else:
                    min_length = "mix_core->chest.width/2 + %d"%((2-s)/2)
                f.write('\tdraw_setup_quad(k, new_pos.ix, new_pos.iy, old_pos.ix, old_pos.iy, %s);\n'%min_length)

                if s < 2:
                    f.write('\tlimb += 3;\n')
                    f.write('\told_pos.x = new_pos.x;\n')
                    f.write('\told_pos.y = new_pos.y;\n')
                    f.write('\told_pos.z = new_pos.z;\n')
                    f.write('\told_pos.ix = new_pos.ix;\n')
                    f.write('\told_pos.iy = new_pos.iy;\n')
    f.write('}\n')
