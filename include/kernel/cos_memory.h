////////////////////////
//
//  Created: Sun Oct 06 2024
//  File: memory.h
//
////////////////////////

#pragma once

#include "cos_memory.h"
#define KERNEL_BASE_ADDRESS 0xFFFFFF8000000000
//The user address space being located in the lower half of memory, this page is the last one.
//It is chosen as the user process stack beginning because it grows downwards (meaning it decreases towards lower addresses).
//PML4 = 255, PUD = 511, PMD = 511, PT = 511
#define PROCESS_STACK_TOP_ADDRESS 0x00007FFFFFFFF000
#include "kernel/multiboot2.h"
#include "types.h"
#include "kernel/idt.h"

extern void *cos_memmove(void *dest, void *src, uint32_t count);
extern void *memset(void *ptr, int character, size_t size);
extern void cos_memcpy(void *dest_buffer, void *source_buffer, unsigned long data_size);
extern uint64_t cos_fork(void);
extern void cos_exit_handler(int64_t status, cos_cpu_stack **stack);
extern void cos_write_handler(uint64_t file_desc, const char *data, size_t size);
noreturn extern void cos_exit(int64_t status);
extern uint64_t cos_mmap_init(unsigned long mbd, struct multiboot_tag_mmap *tag, unsigned long kernel_end_address, uint64_t privileged_addr[]);
extern uint64_t init_reserved_memory(uint64_t framebuffer_addr, uint64_t size);
extern void *cos_malloc(uint64_t size);
extern void cos_free(void *ptr);
extern void print_processes(void);