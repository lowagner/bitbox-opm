// TODO
/*
    have a current_frame, desired_next_frame,
    split it up so that we can have more specialized character air/ground functions.

*/


#include "player.h"
#include "characters.h"
#include "draw.h"
#include "physics.h"
#include <math.h>

struct player players[MAX_PLAYERS] CCM_MEMORY;
unsigned int players_mask CCM_MEMORY;

void player_enable(int player1, int player2)
{
    players_mask = 0;
    if (player1)
    {
        players_mask |= 1;
        players[0].health = 256;
        players[0].lives = 5;
        character_set(0, CHARACTER_opm);
    }
    if (player2)
    {
        players_mask |= 2;
        players[1].health = 256;
        players[1].lives = 5;
        character_set(1, CHARACTER_opm);
    }
}

#include "switch_player.c"

void individual_player_frame(int p, float dt)
{
    players[p].max_vx = players[p].vx;    
    players[p].x += players[p].vx * dt;
    if (players[p].x < map_offset_x)
    {
        players[p].x = map_offset_x;
        players[p].vx = 0;
    }
    else if (players[p].x > barrier_x)
    {
        players[p].x = barrier_x;
        players[p].vx = 0;
    }
    players[p].y += players[p].vy * dt;
    if (players[p].y < 0)
    {
        players[p].y = 0;
        players[p].vy = 0;
    }
    else if (players[p].y >= SCREEN_H - STREET_LEVEL_Y - 1)
    {
        players[p].y = SCREEN_H - STREET_LEVEL_Y - 1;
        players[p].vy = 0;
    }
   
    int previously_in_air = players[p].z < 0;
    players[p].vz += gravity * dt;
    players[p].z += players[p].vz * dt;
    if (players[p].z >= 0 && players[p].vz >= 0)
    {
        players[p].z = 0;
        players[p].vz = 0;
        if (previously_in_air)
        {
            int facing = ANIM_TO(p) & 128;
            animation_interrupt(p, ANIM_CROUCH_R | facing);
            animation_tween(p, dt, 0.4, ANIM_CROUCH_L | facing);
        }
        else
            switch_player_ground(p, dt);
    }
    else
        switch_player_air(p, dt);
    if (players[p].vx > players[p].max_vx)
        players[p].max_vx = players[p].vx;
    players[p].ix = round(players[p].x - map_offset_x);
    players[p].iy = STREET_LEVEL_Y + round(players[p].y + players[p].z);
}

inline void maybe_move_screen(struct player *ahead, struct player *behind, float dt)
{
     if (ahead->x - map_offset_x > 3*SCREEN_W/4 && map_offset_x + SCREEN_W < barrier_x)
     {
        float dx = ahead->max_vx * dt;
        if (dx < 1)
            dx = 1;
        map_offset_x += dx;
        if (map_offset_x + SCREEN_W > barrier_x)
        {
            map_offset_x = barrier_x - SCREEN_W;
            dx = 0;
        }
        if (behind && behind->x <= map_offset_x)
        {
            behind->x = map_offset_x;
            behind->vx = dx/dt;
        }
    }
}

void player_frame(float dt)
{
    if (players[0].health > 0)
    {
        individual_player_frame(0, dt);
        if (players[1].health > 0)
        {
            individual_player_frame(1, dt);
            if (players[0].x > players[1].x)
                maybe_move_screen(&players[0], &players[1], dt);
            else
                maybe_move_screen(&players[1], &players[0], dt);
        }
        else 
            maybe_move_screen(&players[0], NULL, dt);
    }
    else if (players[1].health > 0)
    {
        individual_player_frame(1, dt);
        maybe_move_screen(&players[1], NULL, dt);
    }
    for (int i=2; i<MAX_PLAYERS; ++i)
    {
        if (players[i].health <= 0)
            continue;
        switch_AI_frame(i, dt);
        individual_player_frame(i, dt);
    }
}

void player_init()
{
    for (int i=0; i<MAX_PLAYERS; ++i)
        players[i].health = 0;
    player_enable(1, 0);
}

void player_steal_lives(int recipient, int giver)
{
    if (players[recipient].lives != 0)
        message("was expecting recipient's lives == 0\n");
    if (players[giver].lives == 0)
    {
        message("wasn't expecting giver's lives == 0\n");
        goto plus_continues;
    }
    else if (players[giver].lives == 1)
    {
        plus_continues:
        players[giver].lives += 5;
        players[recipient].lives = 5;
        ++continues;
    }
    else
    {
        players[recipient].lives = players[giver].lives/2;
        players[giver].lives -= players[recipient].lives;
    }
    players[recipient].health = 256;
}

void player_start_level()
{
    message("starting level, player health %f / %f\n", players[0].health, players[1].health);
    // steal lives if you've run out:
    if ((players_mask & 1) && (players[0].health <= 0))
        player_steal_lives(0, 1);
    if ((players_mask & 2) && (players[1].health <= 0))
        player_steal_lives(1, 0);

    //players[2].health = 10;
    //character_set(2, CHARACTER_henchman);
    for (int i=0; i<MAX_PLAYERS; ++i)
    {
        players[i].x = 32+64*i;
        players[i].y = 32*i;
        players[i].z = 0;

        if (players[i].health > 0)
        {
            animation_reset(i, ANIM_IDLE_R, ANIM_IDLE_L);
            draw_add_player(i);
        }
        else
            draw_remove_player(i);

        players[i].vx = 0;
        players[i].vy = 0;
        players[i].vz = 0;

        players[i].thing_grabbed = 0;

        players[i].punch_charge = 0;
        players[i].jump_charge = 0;
        players[i].run_charge = 0;

        switch_player_start_level(i);
    }
}
