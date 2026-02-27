////////////////////////
//
//  Created: Thu Sep 18 2025
//  File: bmp.c
//
////////////////////////

#include "kernel/graphics/bmp.h"
#include "kernel/term/tty.h"
#include "kernel/vga.h"
#include <stdint.h>

static bool load_info_header(image_file_t *image, char *file);
static bool load_info_v2_header(image_file_t *image, char *file);
static bool load_info_v3_header(image_file_t *image, char *file);
static bool load_info_v4_header(image_file_t *image, char *file);
static bool load_info_v5_header(image_file_t *image, char *file);

bool cos_load_bmp_image(image_file_t *image, char *file)
{
    bmp_header_t bmp_header = *(bmp_header_t *)file;

    if (bmp_header.field != BMP_FIELD_BM) {
        cos_printf("BMP field not supported\n");
        return false;
    }

    uint32_t dib_header_size = *(uint32_t *)(file + sizeof(bmp_header_t));

    image->type = IMAGE_FILE_TYPE_BMP;
    image->data = (uint8_t *)file + bmp_header.offset;
    switch (dib_header_size) {
        case sizeof(bmp_bitmap_info_header_t):
            return load_info_header(image, file);
        case sizeof(bmp_bitmap_info_v2_header_t):
            return load_info_v2_header(image, file);
        case sizeof(bmp_bitmap_info_v3_header_t):
            return load_info_v3_header(image, file);
        case sizeof(bmp_bitmap_info_v4_header_t):
            return load_info_v4_header(image, file);
        case sizeof(bmp_bitmap_info_v5_header_t):
            return load_info_v5_header(image, file);
    };

    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_RED, COS_VGA_COLOR_BLACK));
    cos_printf("BMP invalid dib header size: %d\n", dib_header_size);
    cos_term_set_color(COS_VGA_DEFAULT_COLOR);
    return false;
}

static bool load_info_header(image_file_t *image, char *file)
{
    bmp_bitmap_info_header_t info_header = *(bmp_bitmap_info_header_t *)(file + sizeof(bmp_header_t));

    image->width = info_header.width;
    image->height = info_header.height;
    image->bytes_per_pixels = info_header.bits_per_pixels / 8;
    return true;
}

static bool load_info_v2_header(image_file_t *image, char *file)
{
    return load_info_header(image, file);
}

static bool load_info_v3_header(image_file_t *image, char *file)
{
    return load_info_v2_header(image, file);
}

static bool load_info_v4_header(image_file_t *image, char *file)
{
    return load_info_v3_header(image, file);
}

static bool load_info_v5_header(image_file_t *image, char *file)
{
    return load_info_v4_header(image, file);
}
