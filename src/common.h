#pragma once
#include "bitbox.h"
#include "../config.h"

#include <stdint.h>
#include <string.h>

#define gamepad_PRESSED(id, key) (gamepad_old[id] & gamepad_##key)
#define gamepad_PRESS(id, key) (gamepad_new[id] & gamepad_##key)
#define gamepad_any (gamepad_A | gamepad_B | gamepad_X | gamepad_Y | gamepad_start | gamepad_select | gamepad_L | gamepad_R)
#define gamepad_dpad (gamepad_up | gamepad_down | gamepad_left | gamepad_right)
#define gamepad_any_attack (gamepad_A | gamepad_B | gamepad_X | gamepad_L | gamepad_R)

extern uint16_t gamepad_new[MAX_PLAYERS];
extern uint16_t gamepad_AI[MAX_PLAYERS];
extern uint16_t gamepad_old[MAX_PLAYERS];
extern float map_offset_x;
extern int barrier_x;
extern uint8_t day_phase;
extern int pause;
extern int continues;

typedef void fn(void);
inline float sqr(float x)
{
    return x*x;
}
