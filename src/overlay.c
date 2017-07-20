#include "common.h"
#include "font.h"

static uint16_t overlay_mug[27][27] CCM_MEMORY;
static const char *overlay_text = NULL;
static const char *overlay_lines[4] = { NULL, NULL, NULL, NULL };
static uint8_t overlay_lengths[4] = { 0, 0, 0, 0 };
static int overlay_index;
static int overlay_blink;
static fn *overlay_fn;

void overlay_line_increment(const char *new_text_pos);

int overlay_visible()
{
    return (overlay_text != NULL);
}

void overlay_clear()
{
    overlay_text = NULL;     
    overlay_fn = NULL;
    pause = 0;
}

void overlay_dialogue(const char *text, const uint16_t mugshot[27][27], fn *next_fn)
{
    overlay_blink = 0;
    memcpy(overlay_mug, mugshot, sizeof(overlay_mug));
    overlay_text = text;
    overlay_fn = next_fn;

    overlay_index = 0;
    overlay_lines[0] = text;
    overlay_lines[1] = NULL;
    overlay_lines[2] = NULL;
    overlay_lines[3] = NULL;
    overlay_lengths[0] = 0;
    overlay_lengths[1] = 0;
    overlay_lengths[2] = 0;
    overlay_lengths[3] = 0;
}

void overlay_continue(const char *text, const uint16_t mugshot[27][27], fn *next_fn)
{
    overlay_blink = 0;
    memcpy(overlay_mug, mugshot, sizeof(overlay_mug));
    overlay_text = text;
    overlay_fn = next_fn;

    overlay_line_increment(text);
    while (*text == '\n') // don't make the player hit Y for every newline.
        overlay_line_increment(++text);
}

void overlay_picture_line(int line)
{
    uint16_t *dst = &draw_buffer[2*MARGIN_X];
    const uint16_t *src = overlay_mug[line/2];
    for (int i=0; i<27; ++i)
    {
        *dst++ = *src;
        *dst++ = *src++;
    }
}

void overlay_line()
{
    if (vga_line < SCREEN_H-OVERLAY_HEIGHT+MARGIN_Y)
    {
        // start the overlay, a white line then the background color:
        if (vga_line == SCREEN_H-OVERLAY_HEIGHT)
        {
            memset(draw_buffer, 255, 2*SCREEN_W);
            font_set_line_color(65535, overlay_mug[0][0]);
        }
        else if ((vga_line-1)/2 == (SCREEN_H-OVERLAY_HEIGHT)/2)
        {
            uint32_t *dst = (uint32_t *)draw_buffer;
            for (int i=0; i<SCREEN_W/2/4; ++i)
            {
                *dst++ = font_pixel_doubles[0];
                *dst++ = font_pixel_doubles[0];
                *dst++ = font_pixel_doubles[0];
                *dst++ = font_pixel_doubles[0];
            }
        }
    }
    else if (vga_line >= SCREEN_H-OVERLAY_HEIGHT+MARGIN_Y+PICTURE_SIZE)
    {
        // blank the line below the picture (to avoid scanline bleed):
        if ((vga_line)/2 == (SCREEN_H-OVERLAY_HEIGHT+MARGIN_Y+PICTURE_SIZE)/2)
        {
            uint32_t *dst = (uint32_t *)&draw_buffer[2*MARGIN_X];
            for (int i=0; i<(PICTURE_SIZE/2+1)/4; ++i)
            {
                *dst++ = font_pixel_doubles[0];
                *dst++ = font_pixel_doubles[0];
                *dst++ = font_pixel_doubles[0];
                *dst++ = font_pixel_doubles[0];
            }
        }
    }
    else
    {
        int line = vga_line - (SCREEN_H-OVERLAY_HEIGHT+MARGIN_Y);
        overlay_picture_line(line);
        int y = line % LINE_HEIGHT;
        line /= LINE_HEIGHT;
        if (--line < 0 || line >= 4 || !overlay_text[line])
            return;
        font_render_line(overlay_lines[line], overlay_lengths[line], y);
        if (overlay_blink && ((vga_frame/32)%2) && line == overlay_index && y < FONT_HEIGHT)
        {
            uint32_t *dst = (uint32_t *)&draw_buffer[TEXT_OFFSET_X+FONT_WIDTH*(overlay_lengths[line]-1)];
            *dst = ~(*dst);
            ++dst;
            *dst = ~(*dst);
            ++dst;
            *dst = ~(*dst);
        }
    }
}


void overlay_line_increment(const char *new_text_pos)
{
    if (overlay_index == 3)
    {
        overlay_lines[0] = overlay_lines[1];
        overlay_lines[1] = overlay_lines[2];
        overlay_lines[2] = overlay_lines[3];
        overlay_lines[3] = new_text_pos;
        overlay_lengths[0] = overlay_lengths[1];
        overlay_lengths[1] = overlay_lengths[2];
        overlay_lengths[2] = overlay_lengths[3];
        overlay_lengths[3] = 0;
    }
    else
    {
        ++overlay_index;
        overlay_lines[overlay_index] = new_text_pos;
        overlay_lengths[overlay_index] = 0;
    }
}

void overlay_advance_text()
{
    if (++overlay_lengths[overlay_index] > MAX_TEXT_LENGTH)
    {
        const char *new_text_pos = &overlay_lines[overlay_index][--overlay_lengths[overlay_index]];
        char c = *new_text_pos;
        // find a word to break the line at
        if (c == '\n')
            return overlay_line_increment(new_text_pos+1);
        while (c != ' ')
        {
            --overlay_lengths[overlay_index];
            c = *--new_text_pos;
        }
        return overlay_line_increment(new_text_pos+1);
    }
}

void overlay_frame()
{
    // consider next char not yet on the screen:
    char c = overlay_lines[overlay_index][overlay_lengths[overlay_index]];
    if (!c)
    {
        if (new_gamepad[0] & OVERLAY_CLEAR_BUTTON)
        {
            if (overlay_fn)
                overlay_fn();
            else
                overlay_clear();
        }
        else if (overlay_fn && GAMEPAD_PRESS(0, any))
            overlay_fn();

        return;
    }
    else if (c == '\t')
    {
        if (GAMEPAD_PRESS(0, any))
        {
            overlay_blink = 0;
            overlay_advance_text();
        }
        else
            overlay_blink = 1;
        return;
    }
    else if (c == '\n')
    {
        if (GAMEPAD_PRESS(0, any))
            overlay_blink = 0;
        else
        {
            overlay_blink = 1;
            return;
        }
        const char *new_text_pos = &overlay_lines[overlay_index][overlay_lengths[overlay_index]+1];
        overlay_line_increment(new_text_pos);
        while (*new_text_pos == '\n') // don't make the player hit Y for every newline.
            overlay_line_increment(++new_text_pos);
        return;
    }

    if (GAMEPAD_PRESSED(0, Y))
    {
        if (vga_frame % 16)
            return;
    }
    else if (GAMEPAD_PRESSED(0, any))
    {
        if (vga_frame % 2)
            return;
    }
    else if (vga_frame % 4)
        return;

    overlay_advance_text();
}
