#pragma once
#include <stdint.h>

/*
    animation consists of frames of the player's core and limbs;
    these frames are tweened for the final result.

    limbs get converted into hitboxes/quads in draw.
*/

typedef struct animation_absolute
{
    int8_t dx;
    uint8_t dy;
    uint8_t width, height;
}
animation_absolute;

typedef struct animation_limb
{
    uint8_t angle;
    uint8_t length;
}
animation_limb;

#include "switch_animation.h"

void animation_reset(int p, int start_frame, int next_frame);
void animation_interrupt(int p, int frame);
void animation_complete_and_push(int p, int next_frame);
void animation_tween(int p, float delta, float rate, int maybe_next_frame);
