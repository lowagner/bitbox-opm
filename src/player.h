#pragma once
#include <stdint.h>
#include "animation.h"
#include "../config.h"

typedef enum
{
    Wandering=0,
    Thinking,
    Looking,
    Closing,
    DecidingAttack,
    Attacking,
    Circling,
    Special
}
Action;

typedef enum
{
    E=0,
    NE,
    N,
    NW,
    W,
    SW,
    S,
    SE
}
Direction;

struct player
{
    float x, y, z;
    float vx, vy, vz;
    float max_vx;
    
    float inverse_mass;

    int ix, iy;

    unsigned int lives;

    uint8_t character;
    uint8_t thing_grabbed;
    uint8_t index_y;
    uint8_t index_draw;

    uint32_t custom;
    float customf;

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

    struct
    {
        float reaction;
        int target_p;
        Action action;
        Direction direction;
        int thought1;
        int thought2;
    }
    AI;
};

extern struct player players[MAX_PLAYERS];

void player_frame(float dt);
void player_input();
void player_enable(int player1, int player2);

void player_init();
void player_start_level();
void switch_player(int p, int character);

void switch_player_advantage(int k1, int k2, float v2);
void switch_player_disadvantage(int k1, int k2, float v2);

float player_distance2(int p1, int p2);
void AI_set_direction(int p);
void AI_move_to_player(int p, int target, float space);

#define ALLOW_TURN(p) \
        if (gamepad_PRESSED(p, right)) \
            next_flipped = 0; \
        else if (gamepad_PRESSED(p, left)) \
            next_flipped = ANIM_FACE_LEFT
