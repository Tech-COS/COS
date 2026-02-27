////////////////////////
//
//  Created: Sun Mar 16 2025
//  File: command.c
//
////////////////////////

#include "kernel/term/term.h"
#include "kernel/term/commands.h"
#include "kernel/term/tty.h"
#include "kernel/lib/string.h"
#include "../FileSystem/include/wrapper.h"

static uint32_t cos_get_command_argc(const char *command);
static void cos_fill_command_argv(const char *command, char argv[32][20]);
static bool is_separator(const char c, const char *separatros, uint32_t length);
static bool is_command(const char *command, const char *registered_command);
static uint32_t get_word_len(const char *str);
extern void cos_stat_command(int32_t argc, char argv[32][20]);
extern void cos_cd_command(int32_t argc, char argv[32][20]);
extern void cos_ps_command(int32_t argc, char argv[32][20]);

const cos_command COS_COMMANDS[] = {
    {
        "clear", &cos_clear_command
    },
    {
        "launch", &cos_launch_command
    },
    {
        "ls", &cos_ls_command
    },
    {
        "cat", &cos_cat_command
    },
    {
        "mv", &cos_mv_command
    },
    {
        "touch", &cos_touch_command
    },
    {
        "stat", &cos_stat_command
    },
    {
        "lspci", &cos_lspci_command
    },
    {
        "cd", &cos_cd_command
    },
    {
        "pwd", &cos_pwd_command
    },
    {
        "ps", &cos_ps_command
    }
};

// Without a dynamic memory management, each command is limited by 32 arguments of 20 characters
void cos_run_command(char *command)
{
    static size_t nb_commands = sizeof(COS_COMMANDS) / sizeof(cos_command);
    char argv[32][20] = {0};
    int32_t argc = 0;

    if (!command || !cos_strlen(command))
        return;

    for (size_t i = 0; i < nb_commands; ++i) {
        if (is_command(command, COS_COMMANDS[i].command)) {
            argc = cos_get_command_argc(command);
            cos_fill_command_argv(command, argv);
            COS_COMMANDS[i].ptr(argc, argv);
            return;
        }
    }

    command[get_word_len(command)] = '\0';
    cos_printf("cos: %s: command not found\n", command);
}

static uint32_t cos_get_command_argc(const char *command)
{
    uint32_t argc = 0;
    uint32_t index = 0;
    bool last_is_separator = true;

    while (command[index]) {
        if (is_separator(command[index], " \t\0", 3))
            last_is_separator = true;
        else {
            if (last_is_separator)
                ++argc;
            last_is_separator = false;
        }
        ++index;
    }
    return argc;
}

static void cos_fill_command_argv(const char *command, char argv[32][20])
{
    uint32_t argc_index = 0;
    uint32_t index = 0;
    bool last_is_separator = true;

    while (command[index]) {
        if (is_separator(command[index], " \t\0", 3))
            last_is_separator = true;
        else {
            if (last_is_separator) {
                cos_strcpy(argv[argc_index], &command[index]);
                argv[argc_index][get_word_len(argv[argc_index])] = '\0';
                ++argc_index;
            }
            last_is_separator = false;
        }
        ++index;
    }
}

static bool is_separator(const char c, const char *separators, uint32_t length)
{
    for (uint32_t index = 0; index < length; ++index)
        if (separators[index++] == c)
            return true;

    return false;
}

static bool is_command(const char *commad, const char *registered_command)
{
    uint32_t index = 0;

    for (; commad[index] && registered_command[index]; ++index) {
        if (commad[index] != registered_command[index])
            return false;
    }

    return is_separator(commad[index], " \t\0", 3);
}

static uint32_t get_word_len(const char *str)
{
    uint32_t index = 0;

    while (!is_separator(str[index++], " \t\0", 3));
    return index - 1;
}
