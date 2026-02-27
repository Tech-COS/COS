////////////////////////
//
//  Created: Sun Mar 16 2025
//  File: printf.c
//
////////////////////////

#include "kernel/term/tty.h"
#include "kernel/vga.h"
#include "kernel/lib/string.h"
#include <stdarg.h>

static void cos_printnbr_base(uint64_t nbr, const char *prefix, const char *base, uint64_t depth);

// Supported formats: %s %c %d %x %X %o
void cos_printf(const char *str, ...)
{
    va_list list;

    va_start(list, str);
    while (*str) {
        if (*str == '%') {
            switch (*(str + 1)) {
                case 's': {
                    cos_printf(va_arg(list, char *));
                    ++str;
                    break;
                } case 'c': {
                    cos_putchar(va_arg(list, int64_t));
                    ++str;
                    break;
                } case 'd': {
                    cos_printnbr_base(va_arg(list, int64_t), "", "0123456789", 0);
                    ++str;
                    break;
                } case 'x': {
                    cos_printnbr_base(va_arg(list, uint64_t), "0x", "0123456789abcdef", 0);
                    ++str;
                    break;
                } case 'X': {
                    cos_printnbr_base(va_arg(list, uint64_t), "0x", "0123456789ABCDEF", 0);
                    ++str;
                    break;
                } case 'b': {
                    cos_printnbr_base(va_arg(list, uint64_t), "", "0123456789ABCDEF", 0);
                    ++str;
                    break;
                } case 'o': {
                    cos_printnbr_base(va_arg(list, uint64_t), "", "01234567", 0);
                    ++str;
                    break;
                } default: {
                    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_RED, COS_VGA_COLOR_BLACK));
                    cos_printf("cos_printf error: '%c' is not supported\n", *(str + 1));
                    cos_term_set_color(COS_VGA_DEFAULT_COLOR);
                    break;
                }
            }
        } else
            cos_putchar(*str);

        ++str;
    }

    va_end(list);
}

static void cos_printnbr_base(uint64_t nbr, const char *prefix, const char *base, uint64_t depth)
{
    uint64_t base_len = cos_strlen(base);

    if (nbr == 0) {
        cos_printf(prefix);

        if (depth == 0)
            cos_putchar(base[0]);
        return;
    }

    cos_printnbr_base(nbr / base_len, prefix, base, depth + 1);
    cos_putchar(base[nbr % base_len]);
}
