////////////////////////
//
//  Created: Thu Sep 05 2024
//  File: term.h
//
////////////////////////

#pragma once

#include "types.h"

typedef void (*cos_command_ptr)(int32_t argc, char argv[32][20]);

typedef struct cos_command {
    const char *command;
    cos_command_ptr ptr;
} cos_command;

extern const cos_command COS_COMMANDS[];

void cos_run_command(char *command);
