////////////////////////
//
//  Created: Wed Sep 03 2025
//  File: screen.c
//
////////////////////////

#include "kernel/graphics/screen.h"
#include "kernel/cos_memory.h"
#include "kernel/graphics/bmp.h"
#include "kernel/term/tty.h"
#include "kernel/vga.h"
#include "kernel/cos_memory.h"
#include <stdint.h>

static void cos_draw_bmp_image(image_file_t *image, uint32_t pos_x, uint32_t pos_y, uint32_t size_x, uint32_t size_y);

void cos_update_screen()
{
    rgb_screen_t *screen = get_screen();

    if (!screen->screen_buffer)
        return;

    cos_memcpy(screen->screen_buffer, screen->second_buffer, screen->frametag->common.framebuffer_width * screen->frametag->common.framebuffer_height * (screen->frametag->common.framebuffer_bpp >> 3));
}

rgb_screen_t *get_screen()
{
    static rgb_screen_t screen = {0};

    return &screen;
}

void init_screen(struct multiboot_tag_framebuffer *frametag, void *buffer)
{
    rgb_screen_t *screen = get_screen();

    screen->frametag = frametag;
    screen->screen_buffer = buffer;
    screen->second_buffer = cos_malloc(frametag->common.framebuffer_width * frametag->common.framebuffer_height * (frametag->common.framebuffer_bpp >> 3));
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    rgb_screen_t *screen = get_screen();

    if (!screen->frametag)
        return;

    if ((x >= screen->frametag->common.framebuffer_width) || (y >= screen->frametag->common.framebuffer_height))
        return;

    uint32_t *pixels = (uint32_t *)(screen->screen_buffer + y * screen->frametag->common.framebuffer_pitch + x * (screen->frametag->common.framebuffer_bpp >> 3));

    *pixels = color;
}

uint32_t get_pixel(uint32_t x, uint32_t y)
{
    rgb_screen_t *screen = get_screen();

    if (!screen->frametag)
        return 0;

    if ((x >= screen->frametag->common.framebuffer_width) || (y >= screen->frametag->common.framebuffer_height))
        return 0;

    return *(uint32_t *)(screen->second_buffer + y * screen->frametag->common.framebuffer_pitch + x * (screen->frametag->common.framebuffer_bpp >> 3));
}

uint32_t make_color(uint8_t r, uint8_t g, uint8_t b)
{
    rgb_screen_t *screen = get_screen();

    if (!screen->frametag)
        return 0;

    uint32_t r_val = (r * ((1 << screen->frametag->framebuffer_red_mask_size) - 1)) / 255;
    uint32_t g_val = (g * ((1 << screen->frametag->framebuffer_green_mask_size) - 1)) / 255;
    uint32_t b_val = (b * ((1 << screen->frametag->framebuffer_blue_mask_size) - 1)) / 255;

    return (r_val << screen->frametag->framebuffer_red_field_position)
        | (g_val << screen->frametag->framebuffer_green_field_position)
        | (b_val << screen->frametag->framebuffer_blue_field_position);
}

void clear_screen(uint32_t color)
{
    rgb_screen_t *screen = get_screen();

    if (!screen->frametag)
        return;

    uint32_t *fb = (uint32_t *)screen->second_buffer;
    uint32_t total_pixels = screen->frametag->common.framebuffer_width * screen->frametag->common.framebuffer_height * (screen->frametag->common.framebuffer_bpp >> 3);

    memset(fb, color, total_pixels);
}

void extract_color(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b)
{
    rgb_screen_t *screen = get_screen();

    if (!screen->frametag)
        return;

    uint32_t r_val = color >> screen->frametag->framebuffer_red_field_position;
    uint32_t g_val = color >> screen->frametag->framebuffer_green_field_position;
    uint32_t b_val = color >> screen->frametag->framebuffer_blue_field_position;

    if (r)
        *r = r_val * 255 / ((1 << screen->frametag->framebuffer_red_mask_size) - 1);

    if (g)
        *g = g_val * 255 / ((1 << screen->frametag->framebuffer_green_mask_size) - 1);

    if (b)
        *b = b_val * 255 / ((1 << screen->frametag->framebuffer_blue_mask_size) - 1);
}

void draw_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    for (uint32_t pos_x = x; pos_x < x + width; ++pos_x) {
        for (uint32_t pos_y = y; pos_y < y + height; ++ pos_y) {
            put_pixel(pos_x, pos_y, color);
        }
    }
}

uint16_t get_screen_width()
{
    rgb_screen_t *screen = get_screen();

    return screen->frametag->common.framebuffer_width;
}

uint16_t get_screen_height()
{
    rgb_screen_t *screen = get_screen();

    return screen->frametag->common.framebuffer_height;
}

uint16_t get_screen_bytes_per_pixels()
{
    rgb_screen_t *screen = get_screen();

    return screen->frametag->common.framebuffer_pitch;
}

void cos_draw_image(image_file_t *image, uint32_t pos_x, uint32_t pos_y, uint32_t size_x, uint32_t size_y)
{
    switch (image->type) {
        case IMAGE_FILE_TYPE_BMP:
            cos_draw_bmp_image(image, pos_x, pos_y, size_x, size_y);
        default:
            return;
    }
}

void screen_move_up(uint32_t offset)
{
    rgb_screen_t *screen = get_screen();

    if (!screen->screen_buffer)
        return;

    uint32_t *fb = (uint32_t *)screen->screen_buffer;
    uint32_t pitch_bytes = screen->frametag->common.framebuffer_pitch;
    uint32_t pitch_pixels = screen->frametag->common.framebuffer_pitch / 4;
    uint32_t height = screen->frametag->common.framebuffer_height;

    cos_memmove(fb, fb + offset * pitch_pixels, (height - offset) * pitch_bytes);
    memset(fb + (height - offset) * pitch_pixels, 0, offset * pitch_bytes);
}

static void cos_draw_bmp_image(image_file_t *image, uint32_t pos_x, uint32_t pos_y, uint32_t size_x, uint32_t size_y)
{
    rgb_screen_t *screen = get_screen();

    if (!screen->frametag)
        return;

    uint32_t pitch = screen->frametag->common.framebuffer_pitch / 4;
    uint32_t *fb = (uint32_t*)screen->second_buffer;

    uint32_t src_x_map[size_x];
    uint32_t src_y_map[size_y];

    for (uint32_t dx = 0; dx < size_x; dx++)
        src_x_map[dx] = (dx * image->width) / size_x;

    for (uint32_t dy = 0; dy < size_y; dy++)
        src_y_map[dy] = image->height - 1 - ((dy * image->height) / size_y);

    for (uint32_t dy = 0; dy < size_y; dy++) {
        uint32_t dst_y = pos_y + dy;

        if (dst_y >= screen->frametag->common.framebuffer_height)
            break;

        uint32_t src_y = src_y_map[dy];
        uint32_t *dst = fb + dst_y * pitch + pos_x;

        for (uint32_t dx = 0; dx < size_x; dx++) {
            uint32_t dst_x = pos_x + dx;

            if (dst_x >= screen->frametag->common.framebuffer_width)
                break;

            uint32_t src_x = src_x_map[dx];
            uint8_t *pixel = (uint8_t *)&image->data[(src_y * image->width + src_x) * image->bytes_per_pixels];
            uint32_t color;

            if (image->bytes_per_pixels == 3) {
                uint8_t b = pixel[0], g = pixel[1], r = pixel[2];
                color = make_color(r, g, b);
            } else if (image->bytes_per_pixels == 4) {
                uint8_t b = pixel[0], g = pixel[1], r = pixel[2], a = pixel[3];

                if (a == 255)
                    color = make_color(r, g, b);
                else if (a == 0)
                    continue;
                else {
                    uint32_t bg = dst[dx];
                    uint8_t bg_r = (bg >> 16) & 0xFF;
                    uint8_t bg_g = (bg >> 8)  & 0xFF;
                    uint8_t bg_b = (bg >> 0)  & 0xFF;

                    uint32_t alpha = a + 1;
                    uint32_t inv_a = 256 - alpha;

                    uint8_t rr = (r * alpha + bg_r * inv_a) >> 8;
                    uint8_t gg = (g * alpha + bg_g * inv_a) >> 8;
                    uint8_t bb = (b * alpha + bg_b * inv_a) >> 8;

                    color = make_color(rr, gg, bb);
                }
            } else
                continue;

            dst[dx] = color;
        }
    }
}
