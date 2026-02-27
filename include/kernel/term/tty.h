////////////////////////
//
//  Created: Thu Jun 27 2024
//  File: tty.h
//
////////////////////////

#pragma once

#include "kernel/graphics/screen.h"
#include "types.h"

#define CHARACTER_WIDTH_IN_PIXELS 12
#define CHARACTER_HEIGHT_IN_PIXELS 15

typedef struct term_input_buffer {
    char buffer[256];
    size_t index;
} term_input_buffer;

uint32_t get_screen_characters_width();
uint32_t get_screen_characters_height();

// Init
void cos_term_init(void);
void cos_term_set_color(uint8_t color);
void cos_term_clear(void);

// Output
void cos_putchar(char c);
void cos_printf(const char *str, ...);

// Input
void cos_term_clear_input_buffer(void);
void cos_term_input_char(char c);
char *cos_getline(void);
