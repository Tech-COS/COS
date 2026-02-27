//
// Created by user on 1/4/26.
//

#include "kernel/cos_memory.h"
#include "kernel/term/tty.h"
#include "kernel/cos_filesystem.h"
#include "kernel/lib/string.h"

void cos_ps_command(int32_t argc, char argv[32][20])
{
    (void)argv;
    if (1 < argc)
    {
        cos_printf("USAGE: ps\n");
        return;
    }
    print_processes();
}
