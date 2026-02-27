////////////////////////
//
//  Created: Thu Sep 05 2024
//  File: clear.c
//
////////////////////////

#include "kernel/graphics/screen.h"
#include "kernel/term/tty.h"
#include "types.h"

void cos_clear_command(int32_t argc, char argv[32][20])
{
    (void)argc;
    (void)argv;
    cos_term_clear();
    cos_update_screen();
}
