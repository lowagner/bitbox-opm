#!/usr/bin/env python2
from PIL import Image # PIL  is python-imaging package on debian/ubuntu. See http://en.wikipedia.org/wiki/Python_Imaging_Library
import sys
from math import *

"""
create animations:
	4 wide by 8 frames tall in a png file
	create a mk_frames.py for each CHARACTER.
		divide png file dimensions by 4,8 to determine size of each frame.
		8 pixels of the frame height is used as "negative space" below the player;
		8 pixels is added on the left to center things (expect punches forward -- to the right)
		absolute positioning of the core (torso, head, etc.) is based on the zero there.
"""

filename =  sys.argv[1] # '8x16_437.png'
src = Image.open(filename)
name = filename.split("/")[-1][:-4]

frame_width = src.width / 4
frame_height = src.height / 8

core_pixels = dict([((0,255,0), "head"), ((0,192,0), "chest"), ((0,128,0), "torso")])
limb_pixels = dict([
    ((128,192,0), "R_elbow"), ((192,192,0), "R_wrist"), ((255,192,0), "R_knuckle"),
    ((0,192,128), "L_elbow"), ((0,192,192), "L_wrist"), ((0,192,255), "L_knuckle"),
    ((128,128,0), "R_knee"), ((192,128,0), "R_ankle"), ((255,128,0), "R_toe"),
    ((0,128,128), "L_knee"), ((0,128,192), "L_ankle"), ((0,128,255), "L_toe")
])

colors = [0]*16

def RGB16(rgb):
    return ((rgb[0]>>3)<<10) | ((rgb[1]>>3)<<5) | ((rgb[2]>>3))

def limb_print(pos0, limb_coordinates):
    angle0 = 0
    for pos in limb_coordinates:
        #print "// pos %s -> pos %s"%(pos0, pos)
        # angles are strange looking until we remember y is positive downwards:
        angle = round(atan2(pos[1]-pos0[1], pos[0]-pos0[0])*128/pi)
        length = round(((pos[1]-pos0[1])**2 + (pos[0]-pos0[0])**2)**0.5)
        print "{%d, %d},"%(angle-angle0, length)
        angle0 = angle
        pos0 = pos

print "// autogenerated by mk_frames.py", filename
print "// frame size:", frame_width, "x", frame_height
print "const animation_frame frames_%s[ANIM_COUNT] = {"%name
for k in range(32): # ANIM_COUNT
    #print "(animation_frame){"
    print "{"
    x0 = frame_width * (k%4)
    y0 = frame_height * (k/4)
    core = dict()
    limbs = dict()
    for i in range(frame_width):
      for j in range(frame_height):
        pix = src.getpixel((x0+i, y0+j))
        if pix in core_pixels:
            c = core_pixels[pix]
            if c in core:
                core[c].append([i, j])
            else:
                core[c] = [[i, j]]
        elif pix in limb_pixels:
            limbs[limb_pixels[pix]] = [i, j]
    if len(core) != len(core_pixels):
        raise Exception("not enough core information for frame %d"%k)
    if len(limbs) != len(limb_pixels):
        raise Exception("not enough limb information for frame %d"%k)
    core_print = {}
    for c in core:
        coords = core[c]
        if len(coords) != 2:
            raise Exception("no matching (or too many) coordinate pair(s) for core %s in frame %d"%(c, k))
        x_min, x_max = coords[0][0], coords[1][0]
        y_min, y_max = coords[0][1], coords[1][1]
        if x_min > x_max:
            xmax, x_min = x_min, x_max
        width = x_max - x_min + 1
        height = y_max - y_min + 1
        dx = x_min + 8 - frame_width/2
        dy = frame_height - 8 - y_min
        core[c] = [x_min, x_max, y_min, y_max]
        core_print[c] = "{%d, %d, %d, %d},"%(dx, dy, width, height)
    
    print "{"
    for c in [ "head", "chest", "torso" ]:
        print core_print[c]
    print "},"
        
    limb_print(
        [core["chest"][0]-1, core["chest"][2]], 
        [limbs[l] for l in ["R_elbow", "R_wrist", "R_knuckle"]]
    )
    
    limb_print(
        [core["chest"][1]+1, core["chest"][2]],
        [limbs[l] for l in ["L_elbow", "L_wrist", "L_knuckle"]]
    )
    
    limb_print(
        [core["torso"][0], core["torso"][3]+1],
        [limbs[l] for l in ["R_knee", "R_ankle", "R_toe"]]
    )
    
    limb_print(
        [core["torso"][1], core["torso"][3]+1],
        [limbs[l] for l in ["L_knee", "L_ankle", "L_toe"]]
    )
    
    if k == 0:
        colors[1] = RGB16(src.getpixel((core["head"][0]+1, core["head"][2])))
        colors[2] = RGB16(src.getpixel((core["chest"][0]+1, core["chest"][2])))
        colors[3] = RGB16(src.getpixel((core["torso"][0]+1, core["torso"][2])))
        colors[4] = RGB16(src.getpixel((limbs["R_elbow"][0], limbs["R_elbow"][1]-1)))
        colors[5] = RGB16(src.getpixel((limbs["R_wrist"][0], limbs["R_wrist"][1]-1)))
        colors[6] = RGB16(src.getpixel((limbs["R_knuckle"][0], limbs["R_knuckle"][1]-1)))
        colors[7] = RGB16(src.getpixel((limbs["L_elbow"][0], limbs["L_elbow"][1]-1)))
        colors[8] = RGB16(src.getpixel((limbs["L_wrist"][0], limbs["L_wrist"][1]-1)))
        colors[9] = RGB16(src.getpixel((limbs["L_knuckle"][0], limbs["L_knuckle"][1]-1)))
        colors[10] = RGB16(src.getpixel((limbs["R_knee"][0], limbs["R_knee"][1]-1)))
        colors[11] = RGB16(src.getpixel((limbs["R_ankle"][0], limbs["R_ankle"][1]-1)))
        colors[12] = RGB16(src.getpixel((limbs["R_toe"][0]-1, limbs["R_toe"][1])))
        colors[13] = RGB16(src.getpixel((limbs["L_knee"][0], limbs["L_knee"][1]-1)))
        colors[14] = RGB16(src.getpixel((limbs["L_ankle"][0], limbs["L_ankle"][1]-1)))
        colors[15] = RGB16(src.getpixel((limbs["L_toe"][0]-1, limbs["L_toe"][1])))

    print "},"
print "};"

print "const uint16_t colors_%s[16] = {"%name
for i in range(16):
    print "%d,"%colors[i]
print "};"
