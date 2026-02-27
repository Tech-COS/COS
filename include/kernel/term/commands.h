////////////////////////
//
//  Created: Thu Sep 05 2024
//  File: commands.h
//
////////////////////////

#pragma once

#include "types.h"
#include "kernel/cos_filesystem.h"

void cos_clear_command(int32_t argc, char argv[32][20]);
void cos_launch_command(int32_t argc, char argv[32][20]);
void cos_ls_command(int32_t argc, char argv[32][20]);
void cos_cat_command(int32_t argc, char argv[32][20]);
void cos_mv_command(int32_t argc, char argv[32][20]);
void cos_touch_command(int32_t argc, char argv[32][20]);
void cos_mkdir_command(int32_t argc, char argv[32][20]);
void cos_lspci_command(int32_t argc, char argv[32][20]);
void cos_pwd_command(int32_t argc, char argv[32][20]);
uint32_t find_directory_inode(const char *path, bool display_error_messages, dirent2_cos_t *extracted_dirent);

typedef struct BinFileSection
{
    size_t size;
    size_t offset;
    uint64_t address;
} BinFileSection_t;

typedef struct BinFileFunction
{
    char *name;
    size_t hint_or_ordinal;
} BinFileFunction_t;

typedef struct BinFileLibrary
{
    size_t first_thunk;
    char *name;
    BinFileFunction_t *required_functions;
} BinFileImports_t;

typedef struct BinFileData
{
    uint64_t entry_point;
    uint64_t bin_load_address;
    uint64_t stack_size;
    uint64_t heap_size;
    uint64_t number_of_sections;
    uint64_t linkage_table_pointer; //To bind imported functions to the binary in memory, its Import Address Tables must be rewritten.
    BinFileImports_t *imported_libraries;
    BinFileSection_t *sections;
} BinFileData_t;
