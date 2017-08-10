#include "common.h"
#include "draw.h"
#include "player.h"
#include <math.h>

struct quad quads[256] CCM_MEMORY;
int draw_count CCM_MEMORY;
int check_draw_index CCM_MEMORY;

void draw_init()
{
    draw_count = 0;
    for (int i=0; i<256; ++i)
        quads[i].draw_index = 0;
}

#define ORDER(z1, z2) if (z2 < z1) { int z = z2; z2 = z1; z1 = z; }
#define MAY_EXPAND(length, z1) if (length < min_length) { z1 -= (min_length-length)/2; length = min_length; }
#define MAY_EXPAND2(length, z1) if (length < min_##length) { z1 -= (min_##length-length)/2; length = min_##length; }

void draw_setup_quad(int k, int x1, int y1, int x2, int y2, int min_length)
{
    ORDER(y1, y2);
    int height = y2 - y1;
    MAY_EXPAND(height, y1);
    
    ORDER(x1, x2);
    int width = x2 - x1;
    MAY_EXPAND(width, x1);

    quads[k].ix = x1;
    quads[k].iy = y1;
    quads[k].width = width;
    quads[k].height = height;
}

void draw_setup_quad2(int k, int x1, int y1, int x2, int y2, int min_width, int min_height)
{
    ORDER(y1, y2);
    int height = y2 - y1;
    MAY_EXPAND2(height, y1);
    
    ORDER(x1, x2);
    int width = x2 - x1;
    MAY_EXPAND2(width, x1);

    quads[k].ix = x1;
    quads[k].iy = y1;
    quads[k].width = width;
    quads[k].height = height;
}

uint8_t draw_order[256] CCM_MEMORY;

int quad_update_is_alive(struct quad *q, float dt)
{
    if (q->lifetime == 0.0f)
        return 1;
    if (q->lifetime > 0.0f)
    {
        q->lifetime -= dt;
        if (q->lifetime <= 0.0f)
            goto quad_update_kill;
        q->x += q->vx*dt;
        q->y += q->vy*dt;
        q->z += q->vz*dt;
        q->ix = round(q->x - map_offset_x);
        if ((q->ix > SCREEN_W*2) || (SCREEN_W + q->ix + q->width < 0))
            goto quad_update_kill;
        q->iy = STREET_LEVEL_Y + round(q->y + q->z);
    }
    else
    {
        //message("life time is %f\n", q->lifetime);
        if (++q->lifetime >= 0.0f)
            goto quad_update_kill;
    }
    return 1;

    quad_update_kill:
    //message("removing quad %d\n", (int)(q-quads));
    q->draw_index = 0;
    return 0;
}

void draw_update_projectiles(float dt)
{
    int i = 0; // don't use draw_order[0] for anything...
    while (1)
    {
        if (++i > draw_count)
            return;
        struct quad *q = &quads[draw_order[i]];
        //message("checking %d -> %d\n", i, draw_order[i]);
        if (!quad_update_is_alive(q, dt))
            break;
    }
    int delta = 1;
    while (1)
    {
        if (i + delta > draw_count)
            break;
        struct quad *q = &quads[draw_order[i+delta]];
        //message("checking %d -> %d\n", i+delta, draw_order[i+delta]);
        if (quad_update_is_alive(q, dt))
        {
            // quad q is still in the running, bring it down
            draw_order[i] = draw_order[i+delta];
            q->draw_index = i++;
        }
        else
            // quad q is not to be drawn anymore.
            ++delta;
    }
    draw_count -= delta;
}

static inline void swap_draw_order_k_kminus1(int k)
{
    int quad_kminus1 = draw_order[k-1];
    int quad_k = draw_order[k];

    quads[quad_kminus1].draw_index = k;
    quads[quad_k].draw_index = k-1;

    draw_order[k-1] = quad_k;
    draw_order[k] = quad_kminus1;
}

void draw_frame(float dt)
{
    if (dt)
        draw_update_projectiles(dt);
    
    // insertion sort all the drawing quads on the map
    for (int j=2; j<=draw_count; ++j)
    {
        // move quad k as far up as it should be.
        for (int k=j; k > 1 && quads[draw_order[k-1]].iy > quads[draw_order[k]].iy; --k)
            swap_draw_order_k_kminus1(k);
    }
   
    check_draw_index = 1;
    quads[0].next = 0;
    if (vga_frame % 32 == 0)
        message("draw count is %d\n", draw_count);
}

int quads_overlap(int k1, int k2)
{
    // return 1 if quads[k1] overlaps quads[k2], otherwise 0.
    if
    (
        (quads[k1].x + quads[k1].width < quads[k2].x) ||
        (quads[k2].x + quads[k2].width < quads[k1].x) ||
        (quads[k1].z + quads[k1].height < quads[k2].z) ||
        (quads[k2].z + quads[k2].height < quads[k1].z) ||
        (fabs(quads[k1].y - quads[k2].y) > 0.5*(quads[k1].width + quads[k2].width))
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

void draw_line()
{
    if (!draw_count) // none to draw...
        return;
    int16_t vga16 = (int16_t) vga_line;
    // add any new quads to the draw line:
    while (check_draw_index <= draw_count)
    {
        uint8_t current = draw_order[check_draw_index];
        if (quads[current].iy > vga16) 
            break;
        // add current to the (sorted!) linked list
        uint8_t previous = 0;
        uint8_t next = quads[0].next;
        while (next)
        {
            if (quads[next].y > quads[current].y)
                break;
            previous = next;
            next = quads[previous].next;
        }
        quads[previous].next = current;
        quads[current].next = next;
        UNSET_QUAD_COLLIDED(current);

        ++check_draw_index;
    }
    // draw on-screen and currently visible (on this line) quads
    int previous = 0;
    int current = quads[0].next;
    uint16_t *start;
    uint16_t *end;
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
            goto draw_quad;
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
        // draw
        draw_quad:
        if ((current % 16)/2)
        {
            // normal box for core/limbs, etc.
            start = draw_buffer + quads[current].ix;
            end = start + quads[current].width;
        }
        else
        {
            // draw an ellipse (for the head)
            float delta = 2.0*(vga16 - (quads[current].iy + final_iy)*0.5)/quads[current].height;
            delta = 0.5*quads[current].width*sqrtf(1.01f - delta*delta);
            float center_x = quads[current].ix + 0.5*quads[current].width;
            start = draw_buffer + (int)round(center_x - delta);
            end = draw_buffer + (int)round(center_x + delta);
        }
        int draw_left_right = 3;
        if (start >= draw_buffer+SCREEN_W || end < draw_buffer)
            goto next_quad;
        if (start < draw_buffer)
        {
            draw_left_right &= ~1;
            start = draw_buffer;
        }
        if (end >= draw_buffer + SCREEN_W)
        {
            draw_left_right &= ~2;
            end = draw_buffer + SCREEN_W-1;
        }
        if (vga16 == quads[current].iy || vga16 == final_iy)
        {
            while (start <= end)
                *start++ = quads[current].edge_color;
        }
        else
        switch (draw_left_right)
        {
            case 0: // draw neither left nor right borders:
                while (start <= end)
                    *start++ = quads[current].color;
                break;
            case 1: // just left border, not right
                *start++ = quads[current].edge_color; 
                while (start <= end)
                    *start++ = quads[current].color;
                break;
            case 2: // just right border
                while (start < end)
                    *start++ = quads[current].color;
                *start = quads[current].edge_color; 
                break;
            case 3: // both borders
                *start++ = quads[current].edge_color; 
                while (start < end)
                    *start++ = quads[current].color;
                *start = quads[current].edge_color; 
                break;
        }
        next_quad:
        previous = current;
        current = quads[previous].next;
    }
}

void draw_remove_player(int p)
{
    message("removing player %d\n", p);
    int i = 0;
    while (1)
    {
        if (++i > draw_count)
            return;
        if (draw_order[i]/32 == p)
            break;
    }
    int delta = 1;
    while (1)
    {
        if (i + delta > draw_count)
            break;
        if (draw_order[i+delta]/32 == p)
            ++delta;
        else
        {
            draw_order[i] = draw_order[i+delta];
            quads[draw_order[i]].draw_index = i;
            ++i;
        }
    }
    draw_count -= delta;    
}

void draw_explode_player(int p, float lifetime)
{
    message("exploding player %d\n", p);
    int k = 32*p;
    for (int delta=1; delta<32; ++delta)
        quads[++k].lifetime = lifetime;
}

void draw_add_player(int p)
{
    message("adding player %d\n", p);
    // player p must not already have been added.
    draw_add_projectile(32*p+1, 32*p+15);
}

int draw_add_projectile(int k_min, int k_max)
{
    //message("adding projectile from %d to %d, ", k_min, k_max);
    int ret_val = 0;
    static int quads_to_add[16];
    int total_added = 0;
    for (int k=k_min; k<=k_max; ++k)
    {
        if (quads[k].draw_index == 0)
            quads_to_add[total_added++] = k;
        else
            ret_val = 1;
    }
    if (total_added == 0)
    {
        //message("no quad additions necessary!\n");
        return ret_val; // should be 1
    }
    //message("need to add %d quads to draw list\n", total_added);
    int i;
    k_min = quads_to_add[0];
    for (i=1; i<=draw_count; ++i)
    {
        if (quads[draw_order[i]].iy > quads[k_min].iy)
            break;
    }
    // shuffle all previous guys up:
    for (int j=draw_count; j>=i; --j)
    {
        int k = draw_order[j+total_added] = draw_order[j];
        quads[k].draw_index = j + total_added;
    }
    // insert new quads:
    for (int delta=0; delta<total_added; ++delta)
    {
        int k = quads_to_add[delta];
        //message("adding %d\n", k);
        draw_order[i+delta] = k;
        quads[k].draw_index = i+delta;
    }

    draw_count += total_added;
    return ret_val;
}
