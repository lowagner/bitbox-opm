#pragma once
#include <stdint.h>

struct quad
{
    int16_t ix, iy;

    uint8_t width, height;
    uint8_t draw_index;
    uint8_t next;
    
    uint16_t color;
    uint16_t edge_color;
   
    float lifetime;

    float x, y, z;
    float vx, vy, vz;
};

extern struct quad quads[256];

void draw_init();

void draw_frame(float dt);
void draw_line();
void draw_setup_quad(int k, int x1, int y1, int x2, int y2, int min_length);

void draw_remove_player(int p);
void draw_add_player(int p);

void draw_add_projectile(int k_min, int k_max);
