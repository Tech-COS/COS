////////////////////////
//
//  Created: Thu Sep 11 2025
//  File: bmp.h
//
////////////////////////

#pragma once

#include "kernel/graphics/screen.h"
#include "types.h"
#include <stdint.h>

#define IMAGE_FILE_TYPE_BMP 0

#define BMP_FIELD_BM 0x4D42

#define BMP_COMPRESSION_METHOD_RGB 0
#define BMP_COMPRESSION_METHOD_RLE8 1
#define BMP_COMPRESSION_METHOD_RLE4 2
#define BMP_COMPRESSION_METHOD_BITFIELDS 3
#define BMP_COMPRESSION_METHOD_JPEG 4
#define BMP_COMPRESSION_METHOD_PNG 5
#define BMP_COMPRESSION_METHOD_ALPHABITFIELDS 6
#define BMP_COMPRESSION_METHOD_CMYK 11
#define BMP_COMPRESSION_METHOD_CMYKRLE8 12
#define BMP_COMPRESSION_METHOD_CMYKRLE4 13

#define BMP_HALFTONING_ALGORITHM_NONE 0
#define BMP_HALFTONING_ALGORITHM_ERROR_DIFFUSION 1
#define BMP_HALFTONING_ALGORITHM_PANDA 2
#define BMP_HALFTONING_ALGORITHM_SUPER_CIRCLE 3

typedef struct bmp_cie_xyz {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} __attribute__((packed)) bmp_cie_xyz_t;

typedef struct bmp_cie_xyz_triple {
    bmp_cie_xyz_t red;
    bmp_cie_xyz_t green;
    bmp_cie_xyz_t blue;
} __attribute__((packed)) bmp_cie_xyz_triple_t;

typedef struct bmp_header {
    uint16_t field;
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;
} __attribute__((packed)) bmp_header_t;

typedef struct bmp_os_21_bitmap_header {
    uint32_t header_size;
    uint16_t width;
    uint16_t height;
    uint16_t color_planes;
    uint16_t bytes_per_pixels;
} __attribute__((packed)) bmp_os_21_bitmap_header_t;

// Offset : 54 bytes
typedef struct bmp_os_22_bitmap_header {
    uint16_t resolution;
    uint16_t padding;
    uint16_t direction;
    uint16_t halftoning_algorithm;
    uint32_t halftoning_parameter_1;
    uint32_t halftoning_parameter_2;
    uint32_t encoding;
    uint32_t identifier;
} __attribute__((packed)) bmp_os_22_bitmap_header_t;

typedef struct bmp_bitmap_info_header {
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t color_planes;
    uint16_t bits_per_pixels;
    uint32_t compression_method;
    uint32_t image_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t number_of_colors;
    uint32_t number_of_important_color_used;
} __attribute__((packed)) bmp_bitmap_info_header_t;

typedef struct bmp_bitmap_info_v2_header {
    bmp_bitmap_info_header_t info_header;
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
} __attribute__((packed)) bmp_bitmap_info_v2_header_t;

typedef struct bmp_bitmap_info_v3_header {
    bmp_bitmap_info_v2_header_t v2_info_header;
    uint32_t alpha_mask;
} __attribute__((packed)) bmp_bitmap_info_v3_header_t;

typedef struct bmp_bitmap_info_v4_header {
    bmp_bitmap_info_v3_header_t v3_info_header;
    uint32_t cs_type;
    bmp_cie_xyz_triple_t endpoints;
    uint32_t gamma_red;
    uint32_t gamma_green;
    uint32_t gamma_blue;
} __attribute__((packed)) bmp_bitmap_info_v4_header_t;

typedef struct bmp_bitmap_info_v5_header {
    bmp_bitmap_info_v4_header_t v4_info_header;
    uint32_t intent;
    uint32_t profile_data;
    uint32_t profile_size;
    uint32_t reserved;
} __attribute__((packed)) bmp_bitmap_info_v5_header_t;

bool cos_load_bmp_image(image_file_t *image, char *file);
