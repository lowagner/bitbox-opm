#include "animation.h"
#include "player.h"
#include <math.h> // round
#include "draw.h"
#include "common.h"
#include "switch_animation.c"

void animation_reset(int p, int start_frame, int next_frame)
{
    players[p].animation.tween = 0;
    players[p].animation.frames[0] = ANIM_COUNT | (next_frame&128);
    players[p].animation.frames[1] = start_frame;
    players[p].animation.frames[2] = next_frame;
    players[p].animation.mix_from_to = 0 | (1 << 2) | (2 << 4);
    _animation_reset(p);
}

void animation_interrupt(int p, int next_frame)
{
    // go from current mixed state and tween to the next_frame.
    players[p].animation.tween = 0.0;

    // set frame "mix" as "from", set "to" as next_frame.
    // what was "from" will now be the "mix".
    
    // internally, swap mix and from.
    int a_from = players[p].animation.mix_from_to; // swapping!
    int a_mix = (a_from >> 2)&3; // swapping!
    int a_to = (a_from >> 4)&3;
    a_from &= 3;

    players[p].animation.mix_from_to = (a_mix) | (a_from << 2) | (a_to << 4);
    
    players[p].animation.frames[a_mix] = ANIM_COUNT | (next_frame&128); // mix needed to be confessed.
    
    _animation_copy_to(p, a_to, next_frame);
}

void animation_complete_and_push(int p, int next_frame)
{
    // complete current animation, load next_frame in.
    players[p].animation.tween = 0.0;

    // instantly move to what was frame "to" (set as 'from'), set next_frame to 'to'
    // internally, swap from and to, mix can stay the same.
    int a_mix = players[p].animation.mix_from_to;
    int a_from = (a_mix >> 4)&3; // swapping!
    int a_to = (a_mix >> 2)&3; // swapping!
    a_mix &= 3;
    // update:
    players[p].animation.mix_from_to = (a_mix) | (a_from << 2) | (a_to << 4);
    
    _animation_copy_to(p, a_to, next_frame);
}

void animation_pop_and_push(int p, int from_frame, int to_frame)
{
    players[p].animation.tween = 0.0;

    int a_mix, a_from, a_to;
    if (players[p].animation.frames[0] == from_frame)
    {
        a_from = 0;
        if (players[p].animation.frames[1] == to_frame)
        {
            a_to = 1;
            a_mix = 2;
        }
        else
        {
            a_to = 2;
            a_mix = 1;
            _animation_copy_to(p, a_to, to_frame);
        }
    }
    else if (players[p].animation.frames[1] == from_frame)
    {
        a_from = 1;
        if (players[p].animation.frames[0] == to_frame)
        {
            a_to = 0;
            a_mix = 2;
        }
        else
        {
            a_to = 2;
            a_mix = 0;
            _animation_copy_to(p, a_to, to_frame);
        }
    }
    else if (players[p].animation.frames[2] == from_frame)
    {
        a_from = 2;
        if (players[p].animation.frames[0] == to_frame)
        {
            a_to = 0;
            a_mix = 1;
        }
        else
        {
            a_to = 1;
            a_mix = 0;
            _animation_copy_to(p, a_to, to_frame);
        }
    }
    else
    {
        if (players[p].animation.frames[0] == to_frame)
        {
            a_to = 0;
            a_from = 1;
            a_mix = 2;
        }
        else if (players[p].animation.frames[1] == to_frame)
        {
            a_to = 1;
            a_from = 0;
            a_mix = 2;
        }
        else
        {
            a_to = 2;
            a_from = 1;
            a_mix = 0;
            _animation_copy_to(p, a_to, to_frame);
        }
        _animation_copy_to(p, a_from, from_frame);
    }

    players[p].animation.mix_from_to = (a_mix) | (a_from << 2) | (a_to << 4);
}

void animation_tween(int p, float delta, float rate, int maybe_next_frame)
{
    int a_mix = players[p].animation.mix_from_to;
    int a_from, a_to;
    if (rate < 0)
        rate *= -1;
    players[p].animation.tween += delta*rate;
    if  (players[p].animation.tween < 1.0f)
    {
        // get frame locations for 'from' and 'to' from mix_from_to:
        a_from = (a_mix >> 2)&3;
        a_to = (a_mix >> 4)&3;
        a_mix &= 3;
    }
    else
    {
        players[p].animation.tween = 0.0;
        // swap to and from:
        a_to = (a_mix >> 2)&3; // swapped!
        a_from = (a_mix >> 4)&3; // swapped!
        a_mix &= 3;
        // update:
        players[p].animation.mix_from_to = (a_mix) | (a_from << 2) | (a_to << 4);

        _animation_copy_to(p, a_to, maybe_next_frame);
    }
    _animation_tween(p, delta, rate, a_mix, a_from, a_to);
}

