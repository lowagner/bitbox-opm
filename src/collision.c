#include "collision.h"
#include "common.h"
#include "player.h"
#include <math.h> // fabs

int quads_overlap(int k1, int k2)
{
    // return 1 if quads[k1] overlaps quads[k2], otherwise 0.
    if
    (
        (quads[k1].x + quads[k1].width < quads[k2].x) ||
        (quads[k2].x + quads[k2].width < quads[k1].x) ||
        (quads[k1].z + quads[k1].height < quads[k2].z) ||
        (quads[k2].z + quads[k2].height < quads[k1].z) ||
        (fabs(quads[k1].y - quads[k2].y) > quads[k1].width + quads[k2].width)
    )
        return 0;
    return 1;
}

void quads_contest(int k1, int k2)
{
    // TODO: tweak/test this as necessary
    int p1 = k1/32;
    int p2 = k2/32;
    float v1_squared = sqr(quads[k1].vx - players[p2].vx);
    float v2_squared = sqr(quads[k2].vx - players[p1].vx);
    // could contest with vy, but that should be small
    v1_squared += sqr(quads[k1].vz - players[p2].vz);
    v2_squared += sqr(quads[k2].vz - players[p1].vz);

    // take into account player stubbornness and push each other out of the way
    float delta = players[p2].x - players[p1].x;
    float vx_delta, vz_delta;
    if (fabs(delta) < 5)
        vx_delta = quads[k2].vx - quads[k1].vx - 1.0f*delta;
    else
        vx_delta = quads[k2].vx - quads[k1].vx - 25.0f/delta;
    delta = players[p2].z - players[p1].z;
    if (fabs(delta) < 5)
        vz_delta = quads[k2].vz - quads[k1].vz - 1.0f*delta;
    else
        vz_delta = quads[k2].vz - quads[k1].vz - 25.0f/delta;
    players[p1].vx += players[p1].inverse_mass * vx_delta;
    players[p1].vy += players[p1].inverse_mass * vz_delta;
    players[p2].vx -= players[p2].inverse_mass * vx_delta;
    players[p2].vy -= players[p2].inverse_mass * vz_delta;

    float v_delta_squared = sqr(vx_delta)+sqr(vz_delta);
    if (v1_squared < 2.0 && v2_squared < 2.0)
    {
    }
    else if (v1_squared > v2_squared)
    {
        switch_player_advantage(k1, k2, v_delta_squared);
        switch_player_disadvantage(k2, k1, v_delta_squared);
    }
    else
    {
        switch_player_advantage(k2, k1, v_delta_squared);
        switch_player_disadvantage(k1, k2, v_delta_squared);
    }
}

void collision_line()
{
    if (!draw_count) // none to draw...
        return;
    int16_t vga16 = (int16_t) vga_line;
    
    int previous = 0;
    int current = quads[0].next;
    while (current)
    {
        int16_t final_iy = quads[current].iy + quads[current].height;
        // check if we are done drawing quad current:
        if (vga16 > final_iy)
        {
            // delete the previous current from linked list:
            current = quads[previous].next = quads[current].next;
            continue;
        }
        // otherwise...
        // check collisions
        if (QUAD_COLLIDED(current))
            goto next_quad;
        int p = current/32;
        int next = current;
        while ((next = quads[next].next))
        {
            if (p == next/32 || QUAD_COLLIDED(next) || !quads_overlap(current, next))
                continue;
            SET_QUAD_COLLIDED(current);
            SET_QUAD_COLLIDED(next);
            quads_contest(current, next);
            break;
        }
        next_quad:
        previous = current;
        current = quads[previous].next;
    }
}
