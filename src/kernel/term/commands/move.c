////////////////////////
//
// Created: May 18 2025
// File: move.c
//
////////////////////////

#include "types.h"
#include "kernel/cos_filesystem.h"
#include "kernel/lib/string.h"
#include "kernel/term/commands.h"
#include "kernel/term/tty.h"

void cos_memcpy(void *dest_buffer, void *source_buffer, size_t data_size);
void *memset(void *ptr, int character, size_t size);
void cos_strcpy(char *dest, const char *src);

// Extract filename from path
void extract_filename(const char *path, char *filename)
{
    const char *last_slash = path;
    const char *ptr = path;

    // Find last '/'
    while (*ptr) {
        if (*ptr == '/') {
            last_slash = ptr + 1;
        }
        ptr++;
    }

    cos_strcpy(filename, last_slash);
}

// Extract directory path from full path
void extract_directory_path(const char *path, char *dir_path)
{
    const char *last_slash = NULL;
    const char *ptr = path;

    // Find last '/'
    while (*ptr) {
        if (*ptr == '/') {
            last_slash = ptr;
        }
        ptr++;
    }

    if (last_slash == NULL || last_slash == path) {
        // No directory or root
        cos_strcpy(dir_path, "/");
    } else {
        size_t len = last_slash - path;
        cos_strncpy(dir_path, path, len);
        dir_path[len] = '\0';
    }
}

// Find directory entry in a directory and return its position
int find_dirent_position(uint32_t dir_inode, const char *name, dirent2_cos_t *found_entry)
{
    uint8_t content[4096] = {0};
    char entry_name[EXT4_NAME_LEN] = {0};
    short i = 0;
    dirent2_cos_t dir;

    read_wrapper(dir_inode, content, DEFAULT_BLOCK_SIZE, 0, CONTENT_NOT_ACCESSED);

    do {
        cos_memcpy(&dir, &content[i], sizeof(dirent2_cos_t));
        if (!dir.name_len)
            break;

        memset(entry_name, '\0', EXT4_NAME_LEN);
        cos_memcpy(entry_name, &content[sizeof(dirent2_cos_t) + i], dir.name_len);

        if (cos_strcmp(entry_name, name) == 0) {
            cos_memcpy(found_entry, &dir, sizeof(dirent2_cos_t));
            return i; // Return position
        }

        i += dir.rec_len;
    } while (i < 4096);

    return -1;
}

// Remove directory entry from directory
bool remove_dirent_from_directory(uint32_t dir_inode, const char *name)
{
    uint8_t content[4096] = {0};
    char entry_name[EXT4_NAME_LEN] = {0};
    short i = 0;
    dirent2_cos_t dir;
    dirent2_cos_t prev_dir;
    short prev_i = -1;

    read_wrapper(dir_inode, content, DEFAULT_BLOCK_SIZE, 0, CONTENT_NOT_ACCESSED);

    do {
        cos_memcpy(&dir, &content[i], sizeof(dirent2_cos_t));
        if (!dir.name_len)
            break;

        memset(entry_name, '\0', EXT4_NAME_LEN);
        cos_memcpy(entry_name, &content[sizeof(dirent2_cos_t) + i], dir.name_len);

        if (cos_strcmp(entry_name, name) == 0) {
            // Found entry to remove
            if (prev_i >= 0) {
                prev_dir.rec_len += dir.rec_len;
                cos_memcpy(&content[prev_i], &prev_dir, sizeof(dirent2_cos_t));
            } else {
                dir.inode = 0;
                cos_memcpy(&content[i], &dir, sizeof(dirent2_cos_t));
            }

            write_wrapper(dir_inode, content, DEFAULT_BLOCK_SIZE, 0);
            if (dir.file_type == FILE_TYPE_DIRECTORY)
                update_inode_links(dir_inode, -1, FILE_TYPE_DIRECTORY, FILE_TYPE_DIRECTORY);
            else
                update_inode_links(dir_inode, 0, FILE_TYPE_DIRECTORY, FILE_TYPE_REGULAR_FILE);
            return true;
        }

        prev_dir = dir;
        prev_i = i;
        i += dir.rec_len;
    } while (i < 4096);

    return false;
}

// Add directory entry to directory
bool add_dirent_to_directory(uint32_t dir_inode, uint32_t file_inode, const char *name, uint8_t file_type)
{
    uint8_t content[4096] = {0};
    short i = 0;
    dirent2_cos_t dir;
    short name_len = cos_strlen(name);
    short required_len = sizeof(dirent2_cos_t) + name_len;
    // Align to 4 bytes
    required_len = (required_len + 3) & ~3;

    read_wrapper(dir_inode, content, DEFAULT_BLOCK_SIZE, 0, CONTENT_NOT_ACCESSED);

    // Find last entry and check if we can split it
    do {
        cos_memcpy(&dir, &content[i], sizeof(dirent2_cos_t));
        if (!dir.name_len)
            break;

        short actual_len = sizeof(dirent2_cos_t) + dir.name_len;
        actual_len = (actual_len + 3) & ~3; // Align to 4 bytes

        short next_i = i + dir.rec_len;

        // Check if this is the last entry
        if (next_i >= 4096 || content[next_i] == 0) {
            if (dir.rec_len >= actual_len + required_len) {
                dirent2_cos_t new_dir;
                new_dir.inode = file_inode;
                new_dir.rec_len = dir.rec_len - actual_len;
                new_dir.name_len = name_len;
                new_dir.file_type = file_type;

                dir.rec_len = actual_len;
                cos_memcpy(&content[i], &dir, sizeof(dirent2_cos_t));

                short new_pos = i + actual_len;
                cos_memcpy(&content[new_pos], &new_dir, sizeof(dirent2_cos_t));
                cos_memcpy(&content[new_pos + sizeof(dirent2_cos_t)], (void *)name, name_len);

                write_wrapper(dir_inode, content, DEFAULT_BLOCK_SIZE, 0);
                
                update_inode_links(file_inode, 0, FILE_TYPE_REGULAR_FILE, FILE_TYPE_REGULAR_FILE);

                if (new_dir.file_type == FILE_TYPE_DIRECTORY)
                    update_inode_links(dir_inode, 1, FILE_TYPE_DIRECTORY, FILE_TYPE_DIRECTORY);
                else
                    update_inode_links(dir_inode, 0, FILE_TYPE_DIRECTORY, FILE_TYPE_REGULAR_FILE);
                return true;
            }
            break;
        }

        i += dir.rec_len;
    } while (i < 4096);

    return false; // No space available
}

// Main move function
void cos_mv_command(int32_t argc, char argv[32][20])
{
    if (argc < 3) {
        cos_printf("mv: missing file operand\n");
        cos_printf("Usage: mv SOURCE DEST\n");
        return;
    }

    char *source_path = argv[1];
    char *dest_path = argv[2];

    // Extract source directory and filename
    char source_dir_path[256] = {0};
    char source_filename[EXT4_NAME_LEN] = {0};
    extract_directory_path(source_path, source_dir_path);
    extract_filename(source_path, source_filename);

    // Extract destination directory and filename
    char dest_dir_path[256] = {0};
    char dest_filename[EXT4_NAME_LEN] = {0};
    extract_directory_path(dest_path, dest_dir_path);
    extract_filename(dest_path, dest_filename);

    // Find source directory inode
    uint32_t source_dir_inode = find_directory_inode(source_dir_path, false, NULL);
    if (source_dir_inode == 0) {
        cos_printf("mv: cannot access '%s': No such file or directory\n", source_dir_path);
        return;
    }

    // Find destination directory inode
    uint32_t dest_dir_inode = find_directory_inode(dest_dir_path, false, NULL);
    if (dest_dir_inode == 0) {
        cos_printf("mv: cannot access '%s': No such file or directory\n", dest_dir_path);
        return;
    }

    // Check if destination is a directory
    uint32_t dest_check_inode = find_directory_inode(dest_path, false, NULL);
    if (dest_check_inode != 0) {
        dest_dir_inode = dest_check_inode;
        cos_strcpy(dest_filename, source_filename);
    }

    // Find source file entry
    dirent2_cos_t source_entry;
    int source_pos = find_dirent_position(source_dir_inode, source_filename, &source_entry);
    if (source_pos < 0) {
        cos_printf("mv: cannot stat '%s': No such file or directory\n", source_path);
        return;
    }

    // Check if destination already exists
    dirent2_cos_t dest_entry;
    if (find_dirent_position(dest_dir_inode, dest_filename, &dest_entry) >= 0) {
        cos_printf("mv: cannot move '%s' to '%s': File exists\n", source_path, dest_path);
        return;
    }

    // Add entry to destination directory
    if (!add_dirent_to_directory(dest_dir_inode, source_entry.inode, dest_filename, source_entry.file_type)) {
        cos_printf("mv: cannot move '%s' to '%s': No space left on device\n", source_path, dest_path);
        return;
    }

    // Remove entry from source directory
    if (!remove_dirent_from_directory(source_dir_inode, source_filename)) {
        cos_printf("mv: error removing '%s' from source directory\n", source_path);
        remove_dirent_from_directory(dest_dir_inode, dest_filename);
        return;
    }

    cos_printf("mv: '%s' moved to '%s'\n", source_path, dest_path);
}
