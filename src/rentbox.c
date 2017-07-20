#define BUILDING_SPACING 256
#define BUILDING_WIDTH 208
#define ALLEY_WIDTH (BUILDING_SPACING - BUILDING_WIDTH)
#define BUILDING_COLOR 137
#define BUILDING_SIDE_COLOR 9
#define ALLEY_COLOR 41
#define STRIPE_WIDTH 16
#define ROAD_COLOR RGB(100,100,100)
#define STRIPE_COLOR RGB(255,255,0)

void rentbox_init_chain()
{
    overlay_continue("I should wear my superhero outfit.", mug_opm_suave, NULL);
}

void rentbox_init()
{
    message("rentbox init\n");
    barrier_x = 500*SCREEN_W;
    pause = 1;
    overlay_dialogue
    (
        "Time to rent a movie!\0\nMy nephews are coming to Z city.\n\nI think they like superhero shows.", 
        mug_opm_what, NULL //rentbox_init_chain
    );
}

void rentbox_buildings()
{
    int y = vga_line - (STREET_LEVEL_Y - ALLEY_WIDTH);
    
    int x = (int)(round(map_offset_x)) % BUILDING_SPACING;
    int ix;
    if (y < 0)
    {
        if (x < ALLEY_WIDTH)
        {
            ix = ALLEY_WIDTH-x;
            memset(draw_buffer, BUILDING_SIDE_COLOR, 2*ix);
        }
        else
        {
            ix = BUILDING_WIDTH-(x-ALLEY_WIDTH);
            memset(draw_buffer, BUILDING_COLOR, 2*ix);
            goto draw_building_side;
        }
        while (1)
        {
            if (ix + BUILDING_WIDTH >= SCREEN_W)
            {
                memset(draw_buffer+ix, BUILDING_COLOR, 2*(SCREEN_W-ix));
                break;
            }
            memset(draw_buffer+ix, BUILDING_COLOR, 2*BUILDING_WIDTH);
            ix += BUILDING_WIDTH;
            draw_building_side:
            if (ix + ALLEY_WIDTH >= SCREEN_W)
            {
                memset(draw_buffer+ix, BUILDING_SIDE_COLOR, 2*(SCREEN_W-ix));
                break;
            }
            memset(draw_buffer+ix, BUILDING_SIDE_COLOR, 2*ALLEY_WIDTH);
            ix += ALLEY_WIDTH;
        }
    }
    else
    {
        y = ALLEY_WIDTH - y;
        if (x < y)
        {
            ix = y-x;
            memset(draw_buffer, BUILDING_SIDE_COLOR, 2*ix);
            goto draw_alley_ground;
        }
        else if (x < ALLEY_WIDTH)
        {
            ix = ALLEY_WIDTH-x;
            memset(draw_buffer, ALLEY_COLOR, 2*ix);
        }
        else
        {
            ix = BUILDING_WIDTH-(x-ALLEY_WIDTH);
            memset(draw_buffer, BUILDING_COLOR, 2*ix);
            goto draw_alley_with_building_side;
        }
        while (1)
        {
            if (ix + BUILDING_WIDTH >= SCREEN_W)
            {
                memset(draw_buffer+ix, BUILDING_COLOR, 2*(SCREEN_W-ix));
                break;
            }
            memset(draw_buffer+ix, BUILDING_COLOR, 2*BUILDING_WIDTH);
            ix += BUILDING_WIDTH;
            draw_alley_with_building_side:
            if (ix + y >= SCREEN_W)
            {
                memset(draw_buffer+ix, BUILDING_SIDE_COLOR, 2*(SCREEN_W-ix));
                break;
            }
            memset(draw_buffer+ix, BUILDING_SIDE_COLOR, 2*y);
            ix += y;
            draw_alley_ground:
            if (ix - y >= SCREEN_W - ALLEY_WIDTH)
            {
                memset(draw_buffer+ix, ALLEY_COLOR, 2*(SCREEN_W-ix));
                break;
            }
            memset(draw_buffer+ix, ALLEY_COLOR, 2*(ALLEY_WIDTH-y));
            ix += ALLEY_WIDTH-y;
        }
    }
}

void rentbox_street()
{
    if ((vga_line+2)/2 != ((SCREEN_H+STREET_LEVEL_Y)/2)/2)
    {
        uint32_t *dst = (uint32_t *)draw_buffer;
        for (int i=0; i<SCREEN_W/2; ++i)
            *dst++ = ROAD_COLOR | (ROAD_COLOR<<16);
        return;
    }
    uint16_t *dst = draw_buffer;
    const uint16_t *dst_max = draw_buffer + SCREEN_W;
    int x = map_offset_x + (vga_line - STREET_LEVEL_Y)%2;
    x = ((x % (STRIPE_WIDTH*2)) + (STRIPE_WIDTH*2))%(STRIPE_WIDTH*2);
    if (x < STRIPE_WIDTH)
    {
        const int ix = STRIPE_WIDTH - x;
        for (int i=0; i<ix; ++i)
            *dst++ = STRIPE_COLOR;
    }
    else
    {
        const int ix = 2*STRIPE_WIDTH - x;
        for (int i=0; i<ix; ++i)
            *dst++ = ROAD_COLOR;
        goto draw_stripe;
    }
    while (1)
    {
        if (dst + 16 >= dst_max)
        {
            while (dst < dst_max)
                *dst++ = ROAD_COLOR;
            break;
        }
        for (int i=0; i<16; ++i)
            *dst++ = ROAD_COLOR;
        draw_stripe:
        if (dst + 16 >= dst_max)
        {
            while (dst < dst_max)
                *dst++ = STRIPE_COLOR;
            break;
        }
        for (int i=0; i<16; ++i)
            *dst++ = STRIPE_COLOR;
    }
}


void rentbox_background()
{
    if (vga_line < STREET_LEVEL_Y)
        rentbox_buildings();
    else
        rentbox_street();
}
