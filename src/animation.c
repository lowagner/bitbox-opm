#include "animation.h"
#include "player.h"
#include <math.h> // round
#include "draw.h"
#include "common.h"
#include "switch_animation.c"

void animation_reset(int p, int start_frame, int next_frame)
{
    players[p].animation.tween = 0;
    players[p].animation.frames[0] = -1;
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
    
    //players[p].animation.frames[a_from] = -1; // already true, since 'from' was 'mix'
    players[p].animation.frames[a_mix] = -1; // needed to be confessed.
    
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

void animation_tween(int p, const float delta, const float rate, int maybe_next_frame)
{
    int a_mix = players[p].animation.mix_from_to;
    int a_from, a_to;
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

