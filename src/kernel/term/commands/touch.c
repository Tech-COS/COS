#include "kernel/cos_memory.h"
#include "kernel/cos_filesystem.h"
#include "kernel/lib/string.h"

void cos_memcpy(void *dest_buffer, void *source_buffer, unsigned long data_size);
void cos_printf(const char *str, ...);
extern uint32_t cos_generate_crc32c_checksum(uint8_t *bitstream, uint32_t old_checksum, uint32_t data_length);

void generate_file(dirent2_cos_t *entry, char name[EXT4_NAME_LEN])
{
    entry->name_len = cos_strlen(name);
    entry->inode = get_first_null_inode();
    entry->rec_len = 8 + entry->name_len;
    entry->file_type = 1;
}

void cos_touch_command(int32_t argc, char argv[32][20])
{
    if (argc != 2) {
        cos_printf("USAGE: touch file\n");
        return;
    }

    char source_dir_path[256] = {0};
    char source_filename[EXT4_NAME_LEN] = {0};
    extract_directory_path(argv[1], source_dir_path);
    extract_filename(argv[1], source_filename);

    uint32_t source_dir_inode = find_directory_inode(source_dir_path, true, NULL);
    if (source_dir_inode == 0) {
        cos_printf("touch: cannot access '%s': No such file or directory\n", source_dir_path);
        return;
    }

    uint64_t nb_blocks = get_inode_nb_blocks_taken(source_dir_inode);
    char name[EXT4_NAME_LEN] = {0};
    uint8_t content[4096] = {0};
    uint64_t i = 0;
    uint64_t k = 0;
    dirent2_cos_t dir;
    dirent2_cos_t entry;

    for (uint64_t j = 0; j < nb_blocks; ++j) {
        read_wrapper(source_dir_inode, content, DEFAULT_BLOCK_SIZE, j * DEFAULT_BLOCK_SIZE, CONTENT_NOT_ACCESSED);
        do {
            cos_memcpy(&dir, &content[k], sizeof(dirent2_cos_t));
            if (dir.rec_len == 0)
                break;
            memset(name, '\0', EXT4_NAME_LEN);
            cos_memcpy(name, &content[sizeof(dirent2_cos_t) + k], dir.name_len);
            if (!cos_strcmp(name, source_filename)) {
                cos_printf("%s already exist\n", source_filename);
                return;
            }
            //If rm existed, it would need to stop once inode 0 with length != 4096 - directories and name_len >= name_size has been found.
            if (dir.rec_len + k >= 4084)
                break;
            k += dir.rec_len;
        } while (k < 4084);
        if ((dir.rec_len + k >= 4084) || (dir.rec_len == 0))
        {
            if (j < nb_blocks)
            {
                i += k;
                break;
            } else
            {
                //What should be done here would be to add more blocks to the directory but it won't be done for this project.
                cos_printf("Directory out of blocks.\n");
                return;
            }
        }
        k = 0;
        i += DEFAULT_BLOCK_SIZE;
    }

    if (dir.rec_len == 0)
    {
        memset(name, '\0', EXT4_NAME_LEN);
        uint8_t dir_bitstream[16 + DEFAULT_BLOCK_SIZE] = {0};
        cos_memcpy(&dir_bitstream[0], (uint8_t[16]){DEFAULT_UUID_VOLUME}, 16);

        dirent2_cos_tail_t tail;
        tail.det_rec_len = 12;
        tail.det_reserved_zero = 0;
        tail.det_reserved_zero2 = 0;
        tail.det_reserved_ft = 0xDE;
        tail.det_checksum = cos_generate_crc32c_checksum(dir_bitstream, 0, DEFAULT_BLOCK_SIZE + 16);
        write_wrapper(source_dir_inode, (uint8_t *)&tail, 12, (i & ~0xFFF) + 4084);
    }

    uint8_t buffer[600] = {0};
    generate_file(&entry, source_filename);
    entry.name_len = cos_strlen(source_filename);
    entry.rec_len = 4084 - k - dir.name_len - sizeof(dirent2_cos_t);
    entry.rec_len &= ~0b11;

    dir.rec_len = dir.name_len + sizeof(dirent2_cos_t);
    dir.rec_len = dir.rec_len & 0b11 ? dir.rec_len + (4 - (dir.rec_len & 0b11)) : dir.rec_len;

    cos_memcpy(buffer, (uint8_t *)&dir, sizeof(dirent2_cos_t));
    cos_memcpy(&buffer[sizeof(dirent2_cos_t)], name, dir.name_len);
    cos_memcpy(&buffer[dir.rec_len], (uint8_t *)&entry, sizeof(dirent2_cos_t));
    cos_memcpy(&buffer[dir.rec_len + sizeof(dirent2_cos_t)], source_filename, entry.name_len);
    write_wrapper(source_dir_inode, buffer, sizeof(dirent2_cos_t) + entry.name_len + dir.rec_len, i);
}
