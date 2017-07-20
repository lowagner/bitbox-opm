#pragma once

extern float gravity;
extern int barrier_x;

void physics_init();
void physics_slow_time(int amount);
void physics_frame();
