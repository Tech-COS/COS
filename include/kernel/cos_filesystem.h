#pragma once

#include "types.h"
#define LBA28_BIT 1
#define LBA48_BIT 2
#define UDMA_SUPPORT_BIT 4
#define EXT4_NAME_LEN	    255U
#define ROOT_INODE_INDEX 2
#define CONTENT_ACCESSED 1
#define CONTENT_NOT_ACCESSED 0
#define FILE_TYPE_REGULAR_FILE 1
#define FILE_TYPE_DIRECTORY 2

#ifndef DEFAULT_UUID_VOLUME
	#define DEFAULT_UUID_VOLUME	0x0f, 0x63, 0x64,  0x99,  0x7a, 0x63,0x40, 0x77, 0x8d, 0xc5,0x45, 0xf4, 0xac,0xc3, 0x1f, 0x7c
#endif

#ifndef DEFAULT_BLOCK_SIZE
    #define DEFAULT_BLOCK_SIZE 4096
#endif

typedef struct ext4_dir_entry_2_cos {
	uint32_t inode;
	uint16_t rec_len;
	uint8_t name_len;
	uint8_t file_type;
} __attribute__((packed)) dirent2_cos_t;

typedef struct ext4_dir_entry_tail_cos {
	uint32_t det_reserved_zero;
	uint16_t det_rec_len;
	uint8_t det_reserved_zero2;
	uint8_t det_reserved_ft;
	uint32_t det_checksum;
} __attribute__((packed)) dirent2_cos_tail_t;

extern void generate_file(dirent2_cos_t *entry, char name[EXT4_NAME_LEN]);
extern uint8_t discover_disk(uint64_t *max_sectors, bool is_iso);
extern void write_wrapper(uint64_t inode_nb, uint8_t *content, size_t len, size_t offset);
extern void read_wrapper(uint64_t inode_nb, uint8_t *content, size_t len, size_t offset, bool is_content_read);
extern bool read(const char *path, uint8_t *content, size_t len, size_t offset);
extern uint64_t get_inode_nb_blocks_taken(uint64_t inode_nb);
extern uint64_t get_first_null_inode(void);
extern uint64_t update_inode_links(uint64_t inode_nb, int64_t links, uint16_t mode, uint16_t moved_file);
extern bool detect_ext4_partition_start_from_disk(void);
extern void normalize_path(const char* input_path, char* output_path);

void extract_directory_path(const char *path, char *dir_path);
void extract_filename(const char *path, char *filename);
uint32_t find_directory_inode(const char *path, bool display_error_messages, dirent2_cos_t *extracted_dirent);
