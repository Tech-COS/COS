////////////////////////
//
//  Created: Thu Jun 27 2024
//  File: tty.c
//
////////////////////////

#include "kernel/term/tty.h"
#include "kernel/graphics/font.h"
#include "kernel/graphics/screen.h"
#include "kernel/vga.h"
#include "kernel/asm.h"
#include <stddef.h>
#include <stdint.h>

static uint16_t *const COS_VGA_MEMORY = (uint16_t *)0xFFFFFF80000B8000;

static size_t cos_term_row;
static size_t cos_term_column;
static uint8_t cos_term_color;
static uint16_t *cos_term_buffer;
static term_input_buffer cos_input_buffer;

static void cos_term_putentryat(unsigned char c, uint8_t color, size_t x, size_t y);
static void cos_term_scrollup(void);
static void cos_term_clear_last_line(void);
static void cos_update_cursor(void);

uint32_t get_screen_characters_width(void)
{
    rgb_screen_t *screen = get_screen();

    if (screen->second_buffer)
        return (screen->frametag->common.framebuffer_width / CHARACTER_WIDTH_IN_PIXELS) - 1;

    return COS_VGA_WIDTH;
}

uint32_t get_screen_characters_height(void)
{
    rgb_screen_t *screen = get_screen();

    if (screen->second_buffer)
        return (screen->frametag->common.framebuffer_height / CHARACTER_HEIGHT_IN_PIXELS) - 1;

    return COS_VGA_HEIGHT;
}

void cos_term_init(void)
{
    cos_term_buffer = COS_VGA_MEMORY;

    cos_term_clear();
}

void cos_term_set_color(uint8_t color)
{
    cos_term_color = color;
}

void cos_term_clear(void)
{
    rgb_screen_t *screen = get_screen();
    cos_term_row = 0;
    cos_term_column = 0;

    cos_update_cursor();
    cos_term_set_color(COS_VGA_DEFAULT_COLOR);

    if (screen->second_buffer)
        clear_screen(0);
    else
    {
        for (size_t y = 0; y < get_screen_characters_height(); ++y)
        {
            for (size_t x = 0; x < get_screen_characters_width(); ++x)
                cos_term_putentryat(' ', cos_term_color, x, y);
        }
    }
}

void cos_putchar(char c)
{
    switch (c) {
        case '\n':
            cos_term_column = get_screen_characters_width() - 1;
            break;
        case '\b':
            if (cos_term_column)
                cos_term_column -= 2;
            else
                --cos_term_column;

            cos_term_putentryat(' ', cos_term_color, cos_term_column + 1, cos_term_row);
            break;
        default:
            cos_term_putentryat(c, cos_term_color, cos_term_column, cos_term_row);
    }

    if (++cos_term_column == get_screen_characters_width()) {
        cos_term_column = 0;

        if (++cos_term_row == get_screen_characters_height()) {
            cos_term_row = get_screen_characters_height() - 1;
            cos_term_scrollup();
            cos_term_clear_last_line();
        }
    }
    cos_update_cursor();
}

void cos_write_handler(uint64_t file_desc, const char *data, size_t size)
{
    (void)file_desc;
    for (size_t i = 0; i < size; ++i)
        cos_putchar(data[i]);
}

void cos_term_clear_input_buffer(void)
{
    cos_input_buffer.buffer[0] = '\0';
    cos_input_buffer.index = 0;
}

void cos_term_input_char(char c)
{
    if (!c)
        return;

    if (c == '\b') {
        if (cos_input_buffer.index) {
            cos_input_buffer.index -= 1;
            cos_input_buffer.buffer[cos_input_buffer.index] = '\0';
            cos_putchar(c);
        }
    } else {
        cos_input_buffer.buffer[cos_input_buffer.index] = c;
        cos_input_buffer.buffer[cos_input_buffer.index + 1] = '\0';
        cos_input_buffer.index += 1;
        cos_putchar(c);
    }
}

char *cos_getline(void)
{
    if (cos_input_buffer.index && cos_input_buffer.buffer[cos_input_buffer.index - 1] == '\n') {
        cos_input_buffer.buffer[cos_input_buffer.index - 1] = '\0';
        cos_input_buffer.index = 0;
        return cos_input_buffer.buffer;
    }

    return NULL;
}

static void cos_term_putentryat(unsigned char c, uint8_t color, size_t x, size_t y)
{
    cos_term_buffer[y * get_screen_characters_width() + x] = COS_VGA_ENTRY(c, color);
    put_font_char(c, x * CHARACTER_WIDTH_IN_PIXELS, y * CHARACTER_HEIGHT_IN_PIXELS, make_color(255, 255, 255));
}

static void cos_term_scrollup(void)
{
    // Scroll up with VGA Mode
    for (size_t y = 0; y < get_screen_characters_height() - 1; ++y)
    {
        for (size_t x = 0; x < get_screen_characters_width(); ++x)
            cos_term_buffer[y * get_screen_characters_width() + x] = cos_term_buffer[(y + 1) * get_screen_characters_width() + x];
    }

    // Scroll up with graphical mode
    if (get_screen()->screen_buffer)
        screen_move_up(CHARACTER_HEIGHT_IN_PIXELS);
}

static void cos_term_clear_last_line(void)
{
    for (size_t x = 0; x < get_screen_characters_width(); ++x)
        cos_term_buffer[(get_screen_characters_height() - 1) * get_screen_characters_width() + x] = COS_VGA_ENTRY(' ', cos_term_color);
}

static void cos_update_cursor(void)
{
    uint16_t position = cos_term_row * get_screen_characters_width() + cos_term_column;

    cos_outb(0x3D4, 0x0F);
    cos_outb(0x3D5, (uint8_t)(position & 0xFF));
    cos_outb(0x3D4, 0x0E);
    cos_outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}
