////////////////////////
//
//  Created: Thu Jun 27 2024
//  File: vga.h
//
////////////////////////

#pragma once

#include "types.h"
#include <stdint.h>

#define COS_VGA_WIDTH 80
#define COS_VGA_HEIGHT 25

#define COS_VGA_ENTRY_COLOR(fg, bg) ((fg) | (bg) << 4)
#define COS_VGA_ENTRY(uc, color) (((uint16_t)uc) | ((uint16_t)color) << 8)

enum vga_color {
	COS_VGA_COLOR_BLACK = 0,
	COS_VGA_COLOR_BLUE = 1,
	COS_VGA_COLOR_GREEN = 2,
	COS_VGA_COLOR_CYAN = 3,
	COS_VGA_COLOR_RED = 4,
	COS_VGA_COLOR_MAGENTA = 5,
	COS_VGA_COLOR_BROWN = 6,
	COS_VGA_COLOR_LIGHT_GREY = 7,
	COS_VGA_COLOR_DARK_GREY = 8,
	COS_VGA_COLOR_LIGHT_BLUE = 9,
	COS_VGA_COLOR_LIGHT_GREEN = 10,
	COS_VGA_COLOR_LIGHT_CYAN = 11,
	COS_VGA_COLOR_LIGHT_RED = 12,
	COS_VGA_COLOR_LIGHT_MAGENTA = 13,
	COS_VGA_COLOR_LIGHT_BROWN = 14,
	COS_VGA_COLOR_WHITE = 15,
};

#define COS_VGA_DEFAULT_COLOR COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK)
