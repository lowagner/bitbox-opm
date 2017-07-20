#include "physics.h"
#include "player.h"
#include "common.h"

float gravity;
int physics_wait;

void physics_init()
{
    gravity = 1.0;
    barrier_x = SCREEN_W*2;
    physics_wait = 1;
}

void physics_slow_time(int amount)
{
    if (amount <= 0)
        return;
    physics_wait = amount > physics_wait ? amount : physics_wait;
}

void physics_frame()
{
    player_frame(1.0/physics_wait);
    if (vga_frame % 32 == 0)
        message("delta %02d\n", physics_wait);
    if (physics_wait > 1)
        --physics_wait;
}
