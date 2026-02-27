////////////////////////
//
//  Created: Wed Sep 03 2025
//  File: font.c
//
////////////////////////

#include "kernel/graphics/font.h"
#include "kernel/graphics/screen.h"
#include "kernel/cos_memory.h"
#include <limits.h>
#include <stdint.h>

extern char _binary_assets_font_lat9u_psf_start;
extern char _binary_assets_font_lat9u_psf_end;

uint16_t cos_font_unicode[USHRT_MAX] __attribute__ ((aligned));

bool psf_init()
{
    uint16_t glyph = 0;
    PSF_font_t *font = (PSF_font_t *)&_binary_assets_font_lat9u_psf_start;

    if (font->flags == 0)
        return false;

    char *s = (char *)(
    (unsigned char*)&_binary_assets_font_lat9u_psf_start +
      font->header_size +
      font->num_glyph * font->bytes_per_glyph
    );

    memset(cos_font_unicode, 0, sizeof(cos_font_unicode));
    while((unsigned char *)s < (unsigned char*)&_binary_assets_font_lat9u_psf_end) {
        uint16_t uc = *((unsigned char *)(&s[0]));
        if((uc & 0xFF) == 0xFF) {
            glyph++;
            s++;
            continue;
        } else if(uc & 128) {
            /* UTF-8 to cos_font_unicode */
            if((uc & 32) == 0 ) {
                uc = ((s[0] & 0x1F)<<6)+(s[1] & 0x3F);
                s++;
            } else
            if((uc & 16) == 0 ) {
                uc = ((((s[0] & 0xF)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F);
                s+=2;
            } else
            if((uc & 8) == 0 ) {
                uc = ((((((s[0] & 0x7)<<6)+(s[1] & 0x3F))<<6)+(s[2] & 0x3F))<<6)+(s[3] & 0x3F);
                s+=3;
            } else
                uc = 0;
        }
        /* save translation */
        cos_font_unicode[uc] = glyph;
        s++;
    }

    return true;
}

void put_font_char(uint16_t character, uint16_t pos_x, uint16_t pos_y, uint32_t color)
{
    PSF_font_t *font = (PSF_font_t *)&_binary_assets_font_lat9u_psf_start;
    int bytes_per_lines = (font->width + 7) / 8;
    bool clear_character = character == ' ';

    character = cos_font_unicode[character];
    uint8_t *glyph =
        (uint8_t *)&_binary_assets_font_lat9u_psf_start
        + font->header_size
        + (character > 0 && character < font->num_glyph ? character : 0) * font->bytes_per_glyph;

    for (uint16_t y = 0; y < font->height; ++y) {
        uint32_t mask = 1 << (font->width - 1);

        for (uint16_t x = 0; x < font->width; ++x) {
            if (clear_character)
                put_pixel(x + pos_x, y + pos_y, 0);
            else if (*((uint32_t *)glyph) & mask)
                put_pixel(x + pos_x, y + pos_y, color);

            mask >>= 1;
        }

        glyph += bytes_per_lines;
    }
}
