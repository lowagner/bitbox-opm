#pragma once
#include "bitbox.h"
#include "../config.h"

#include <stdint.h>
#include <string.h>

#define GAMEPAD_PRESS(id, key) (new_gamepad[id] & gamepad_##key)
#define gamepad_any (gamepad_A | gamepad_B | gamepad_X | gamepad_Y | gamepad_start | gamepad_select | gamepad_L | gamepad_R)
#define gamepad_dpad (gamepad_up | gamepad_down | gamepad_left | gamepad_right)

extern uint16_t new_gamepad[2];
extern uint16_t old_gamepad[2];
extern float map_offset_x;
extern int barrier_x;
extern uint8_t day_phase;
extern int pause;
extern int continues;

typedef void fn(void);
