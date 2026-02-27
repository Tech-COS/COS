/*
** EPITECH PROJECT, 2025
** COS
** File description:
** pwd
*/

#include "types.h"
#include "../../../../MemoryManagement/include/cos_memory_management.h"
#include "kernel/cos_filesystem.h"
#include "kernel/lib/string.h"
#include "kernel/term/tty.h"
#include "../FileSystem/include/env.h"

void cos_pwd_command(int32_t argc, char argv[32][20])
{
    UNUSED(argc);
    UNUSED(argv);
    char* path = get_env_path();
    cos_printf("%s\n",path);
}

