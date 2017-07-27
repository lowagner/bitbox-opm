#include "common.h"
#include "draw.h"
#include <math.h>

struct quad quads[256] CCM_MEMORY;
int draw_count CCM_MEMORY;
int check_draw_index CCM_MEMORY;

void draw_setup_quad(int k, int x1, int y1, int x2, int y2, int min_length)
{
    #define ORDER(z1, z2) if (z2 < z1) { int z = z2; z2 = z1; z1 = z; }
    #define MAY_EXPAND(length, z1) if (length < min_length) { z1 -= (min_length-length)/2; length = min_length; }

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

uint8_t draw_order[256] CCM_MEMORY;

static inline void swap_draw_order_k_kminus1(int k)
{
    int quad_kminus1 = draw_order[k-1];
    int quad_k = draw_order[k];

    quads[quad_kminus1].draw_index = k;
    quads[quad_k].draw_index = k-1;

    draw_order[k-1] = quad_k;
    draw_order[k] = quad_kminus1;
}

void draw_remove_old_projectiles(float dt)
{
    int i=-1;
    while (1)
    {
        if (++i >= draw_count)
            return;
        struct quad *q = &quads[draw_order[i]];
        if (!q->lifetime || (q->lifetime -= dt, q->lifetime > 0))
            continue;
        break;
    }
    int delta = 1;
    while (1)
    {
        if (i + delta >= draw_count)
            break;
        struct quad *q = &quads[draw_order[i+delta]];
        if (!q->lifetime || (q->lifetime -= dt, q->lifetime > 0))
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

void draw_frame(float dt)
{
    if (dt)
        draw_remove_old_projectiles(dt);
    
    // insertion sort all the drawing quads on the map
    for (int j=0; j<draw_count-1; ++j)
    {
        // move quad k as far up as it should be.
        for (int k=j+1; k > 0 && quads[draw_order[k-1]].iy > quads[draw_order[k]].iy; --k)
            swap_draw_order_k_kminus1(k);
    }
   
    check_draw_index = 0;
    quads[0].next = 0;
    if (vga_frame % 32 == 0)
        message("draw count is %d\n", draw_count);
}

void draw_line()
{
    if (!draw_count) // none to draw...
        return;
    int16_t vga16 = (int16_t) vga_line;
    // add any new quads to the draw line:
    while (check_draw_index < draw_count)
    {
        uint8_t current = draw_order[check_draw_index];
        if (quads[current].iy <= vga16) 
        {
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

            ++check_draw_index;
        }
        else
            break;
    }
    // draw on-screen and currently visible (on this line) quads
    uint8_t previous = 0;
    uint8_t current = quads[0].next;
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
        uint16_t *start;
        uint16_t *end;
        if (current % 32 != 1)
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
    int i=-1;
    while (1)
    {
        if (++i >= draw_count)
            return;
        if (draw_order[i]/32 == p)
            break;
    }
    int delta = 1;
    while (1)
    {
        if (i + delta >= draw_count)
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

void draw_add_player(int p)
{
    // player p must not already have been added.
    draw_add_projectile(32*p+1, 32*p+15);
}

void draw_add_projectile(int k_min, int k_max)
{
    int i;
    for (i=0; i<draw_count; ++i)
    {
        if (quads[draw_order[i]].iy > quads[k_min].iy)
            break;
    }
    int delta = k_max - k_min + 1;
    for (int j=draw_count-1; j>=i; --j)
    {
        draw_order[j+delta] = draw_order[j];
        quads[draw_order[j+delta]].draw_index = j + delta;
    }

    for (int j=0; j<delta; ++j)
    {
        draw_order[i+j] = k_min+j;
        quads[k_min+j].draw_index = i+j;
    }

    draw_count += delta;
}
