#pragma once
#include <stdint.h>
#include "animation.h"

#define MAX_PLAYERS 6

struct player
{
    float x, y, z;
    float vx, vy, vz;
    float max_vx;

    int ix, iy;

    unsigned int lives;

    uint8_t character;
    uint8_t thing_grabbed;
    uint8_t index_y;
    uint8_t index_draw;

    uint32_t custom;

    float health;

    float stamina_punch;
    float stamina_kick;
    float stamina_grab;

    float punch_charge;
    float jump_charge;
    float run_charge;
    float kick_charge;

    struct animation animation;
    const struct animation_frame *all_frames;
};

extern struct player players[MAX_PLAYERS];

void player_frame(float dt);
void player_input();
void player_enable(int player1, int player2);

void player_init();
void player_start_level();
void switch_player(int p, int character);

#define ALLOW_TURN(p) \
        if (GAMEPAD_PRESSED(p, right)) \
            next_flipped = 0; \
        else if (GAMEPAD_PRESSED(p, left)) \
            next_flipped = ANIM_FACE_LEFT
