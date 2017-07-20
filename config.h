#pragma once

#define MAX_PHYSICS_WAIT 256

#define STREET_LEVEL_Y (SCREEN_H/2+10)
#define OVERLAY_CLEAR_BUTTON (gamepad_A|gamepad_start)

// don't mess with these unless you change a lot of things:
#define FONT_HEIGHT 7
#define FONT_WIDTH 6
#define LINE_HEIGHT (FONT_HEIGHT+2)

// you can mess with these:
#define OVERLAY_HEIGHT 62
#define MAX_TEXT_LENGTH 36
#define PICTURE_SIZE 54
#define INNER_MARGIN_X 12
#define MARGIN_X ((SCREEN_W - PICTURE_SIZE - INNER_MARGIN_X - MAX_TEXT_LENGTH*FONT_WIDTH)/3)
#define MARGIN_Y ((OVERLAY_HEIGHT-TEXT_HEIGHT)/2)

// don't mess after here:
#define TEXT_OFFSET_X (2*MARGIN_X+PICTURE_SIZE+INNER_MARGIN_X)
#define TEXT_HEIGHT (6*LINE_HEIGHT)
#if (TEXT_HEIGHT != PICTURE_SIZE)
#error "TEXT_HEIGHT should be = PICTURE_SIZE"
#endif

#if (TEXT_OFFSET_X % 2)
#error "TEXT_OFFSET_X must be even."
#endif


#define SCREEN_W VGA_H_PIXELS
#define SCREEN_H VGA_V_PIXELS

