////////////////////////
//
// Created: Sun Mar 16 2025
// File: ls.c
//
////////////////////////
#include "types.h"
#include "kernel/cos_filesystem.h"
#include "kernel/lib/string.h"
#include "kernel/term/tty.h"
#include "../FileSystem/include/env.h"

void cos_memcpy(void *dest_buffer, void *source_buffer, size_t data_size);
void *memset(void *ptr, int character, size_t size);

// Find inode with path
uint32_t find_directory_inode(const char *path, bool display_error_messages, dirent2_cos_t *extracted_dirent)
{
    if (!path || cos_strlen(path) == 0 || (cos_strlen(path) == 1 && path[0] == '/')) {
        return 2; // Inode 2 = root
    }

    uint32_t current_inode = 2; // Start at root
    char path_copy[256] = {0};
    char *token;
    uint8_t content[4096] = {0};
    dirent2_cos_t dir;
    char name[EXT4_NAME_LEN] = {0};

    cos_strcpy(path_copy, path);

    char *path_ptr = path_copy;
    if (path_ptr[0] == '/') {
        path_ptr++;
    }

    token = cos_strtok(path_ptr, "/");
    while (token != NULL) {
        read_wrapper(current_inode, content, DEFAULT_BLOCK_SIZE, 0, CONTENT_NOT_ACCESSED);

        bool found = false;
        short i = 0;

        do {
            cos_memcpy(&dir, &content[i], sizeof(dirent2_cos_t));
            if (!dir.name_len)
                break;

            memset(name, '\0', EXT4_NAME_LEN);
            cos_memcpy(name, &content[sizeof(dirent2_cos_t) + i], dir.name_len);

            if (cos_strcmp(name, token) == 0) {
                current_inode = dir.inode;
                found = true;

                if (extracted_dirent)
                    *extracted_dirent = dir;

                break;
            }

            i += dir.rec_len;
        } while (i < 4096);

        if (!found) {
            if (display_error_messages)
                cos_printf("ls: cannot access '%s': No such file or directory\n", path);

            return 0;
        }

        token = cos_strtok(NULL, "/");
    }

    return current_inode;
}

// List a directory from an inode
void list_directory_content(uint32_t dir_inode)
{
    uint8_t content[4096] = {0};
    char name[EXT4_NAME_LEN] = {0};
    short i = 0;
    dirent2_cos_t dir;

    read_wrapper(dir_inode, content, DEFAULT_BLOCK_SIZE, 0, CONTENT_ACCESSED);

    do {
        cos_memcpy(&dir, &content[i], sizeof(dirent2_cos_t));
        if (!dir.name_len)
            break;
        memset(name, '\0', EXT4_NAME_LEN);
        cos_memcpy(name, &content[sizeof(dirent2_cos_t) + i], dir.name_len);
        cos_printf("%s\n", name);

        i += dir.rec_len;
    } while (i < 4096);
}

void cos_ls_command(int32_t argc, char argv[32][20])
{
    char *path = get_env_path();
    dirent2_cos_t extracted_dirent;

    if (argc > 1) {
        path = argv[1];
    }

    uint32_t dir_inode = find_directory_inode(path, true, &extracted_dirent);

    if (dir_inode == 0) {
        return;
    }

    if (extracted_dirent.file_type == FILE_TYPE_REGULAR_FILE)
        cos_printf("%s\n", path);
    else
        list_directory_content(dir_inode);
}
