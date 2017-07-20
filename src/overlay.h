#pragma once
#include <stdint.h>

int overlay_visible();
void overlay_line();
void overlay_frame();

void overlay_dialogue(const char *text, const uint16_t mugshot[27][27], fn *next);
void overlay_continue(const char *text, const uint16_t mugshot[27][27], fn *next);
void overlay_clear();
