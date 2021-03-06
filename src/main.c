#include "common.h"
#include "overlay.h"
#include "player.h"
#include "draw.h"
#include "physics.h"
#include "collision.h"

#include "font.h"
#include "levels.h"
#include "mugs.h"

uint16_t gamepad_new[MAX_PLAYERS] CCM_MEMORY;
uint16_t gamepad_AI[MAX_PLAYERS] CCM_MEMORY;
uint16_t gamepad_old[MAX_PLAYERS] CCM_MEMORY;
float map_offset_x CCM_MEMORY;
int barrier_x CCM_MEMORY;
Level level CCM_MEMORY;
uint8_t day_phase CCM_MEMORY;
int pause CCM_MEMORY;
int continues;

void game_switch(Level new_level)
{
    if (new_level == level)
        return;
    
    for (int i=2; i<MAX_PLAYERS; ++i)
        gamepad_AI[i] = 0;

    day_phase = 0;
    level = new_level;
    switch (new_level)
    {
        case level_rentbox:
            rentbox_init();
            break;
        default:
            break;
    }
    player_start_level();
}

void game_init()
{
    continues = 0;
    pause = 0;
    map_offset_x = 0;
    font_init();
    physics_init();
    draw_init();
    player_init();
    level = level_none;
    game_switch(level_rentbox);
}

void game_frame()
{
    // update controllers, both for player and for AI
    // what has been pressed that's new?
    gamepad_new[0] = gamepad_buttons[0] & (~gamepad_old[0]);
    gamepad_new[1] = gamepad_buttons[1] & (~gamepad_old[1]);
    uint32_t *new = (uint32_t *)gamepad_new;
    uint32_t *current = (uint32_t *)gamepad_AI;
    for (int i=1; i<MAX_PLAYERS/2; ++i)
        *++new = *++current;
    
    // update old controllers to what was current.
    gamepad_old[0] = gamepad_buttons[0];
    gamepad_old[1] = gamepad_buttons[1];
    uint32_t *old = (uint32_t *)gamepad_old;
    current = (uint32_t *)gamepad_AI;
    for (int i=1; i<MAX_PLAYERS/2; ++i)
        *++old = *++current;

    if (overlay_visible())
        overlay_frame();

    if (pause)
        return draw_frame(0);

    physics_frame();
}

void game_snd_buffer(uint16_t* buffer, int len)
{
    for (int i=0; i<len; ++i)
    {
        buffer[i] = 128 | (128u<<8);
    }
}

void graph_line()
{
    if (vga_odd)
        return collision_line();
    if (vga_line < OVERLAY_HEIGHT && overlay_visible())
        return overlay_line();
    switch (level)
    {
        case level_rentbox:
            rentbox_background();
            break;
        default:
            if (vga_line/2 == 0)
                memset(draw_buffer, 0, 2*SCREEN_W);    
            break;
    }
    draw_line();
    if (vga_line/8 == 1)
    {
        static char buffer[4] = {0};
        if (vga_line == 8)
        {
            int inverse_dt = physics_inverse_dt();
            buffer[0] = '0'+(inverse_dt/100)%10;
            buffer[1] = '0'+(inverse_dt/10)%10;
            buffer[2] = '0'+inverse_dt%10;
        }
        font_set_line_color(RGB(255,255,255),0);
        font_render_line(buffer, 3, vga_line-8);
    }
}
