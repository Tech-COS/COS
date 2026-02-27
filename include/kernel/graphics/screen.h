////////////////////////
//
//  Created: Wed Sep 03 2025
//  File: screen.h
//
////////////////////////

#pragma once

#include "kernel/multiboot2.h"
#include <stdint.h>

typedef struct rgb_screen {
    struct multiboot_tag_framebuffer *frametag;
    uint8_t *screen_buffer;
    uint8_t *second_buffer;
} rgb_screen_t;

typedef struct image_file {
    uint16_t type;
    uint32_t width;
    uint32_t height;
    uint16_t bytes_per_pixels;
    uint8_t *data;
} image_file_t;

void cos_update_screen();
rgb_screen_t *get_screen();
void init_screen(struct multiboot_tag_framebuffer *frametag, void *buffer);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t get_pixel(uint32_t x, uint32_t y);
uint32_t make_color(uint8_t r, uint8_t g, uint8_t b);
void clear_screen(uint32_t color);
void extract_color(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b);
void draw_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
uint16_t get_screen_width();
uint16_t get_screen_height();
uint16_t get_screen_bytes_per_pixels();
void cos_draw_image(image_file_t *image, uint32_t pos_x, uint32_t pos_y, uint32_t size_x, uint32_t size_y);
void screen_move_up(uint32_t offset);
