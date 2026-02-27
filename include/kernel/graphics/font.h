////////////////////////
//
//  Created: Wed Sep 03 2025
//  File: font.h
//
////////////////////////

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct PSF_font {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t num_glyph;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width;
} PSF_font_t;

bool psf_init();
void put_font_char(uint16_t character, uint16_t pos_x, uint16_t pos_y, uint32_t color);
