#pragma once
#include <stdint.h>

// The Global Descriptor table defines the properties of the segments used by the OS.
// By default, we must define the kernel code segment that will be loaded in CS as well as a data segment for the data segment registers.
// In 64bit mode, the data segments are ignored except for FS and GS and most parameters which must be defined in 32bit mode are ignored because address space is considered infinite.
// The first entry of this table is always NULL.

// Macros for readability.

#define ACCESSED 1ULL << 0    //The CPU switches this bit to 1 when the specified segment is accessed. If the segment is stored in Read Only Data and the segment is accessed, it leads to failure.
#define RW		 1ULL << 1    //Sets the Read Write bit. In code segments, it enables read. In data segments, it enables write.
#define EXEC	 1ULL << 3    //Executable Bit. It defines the segment as executable.
#define DESCTYPE 1ULL << 4    //Descriptor type bit. When set to 0, it is a system segment. When set to 1, it is a data or code segment.
#define DPLLOW	 1ULL << 5    //Defines the Ring of the segment. Ring 0 is kernel, Ring 3 is userland.
#define DPLHIGH	 1ULL << 6    //Ring 1 and 2 are never used by a modern OS.
#define PRESENT  1ULL << 7    //Sets if a segment is valid or not. Must be switched on for all segments.
#define LONGBIT	 1ULL << 9    //Enables long mode for a segment.

typedef struct cos_gdt_entry {
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t flags0;
    uint8_t limit1: 4;
    uint8_t flags1: 4;
    uint8_t base2;
} __attribute__((packed)) cos_gdt_entry_t;

typedef struct cos_tss_segment_high {
    uint32_t base3;
    uint32_t reserved2;
} __attribute__((packed)) cos_gdt_entry_tss_high_t;

typedef struct task_segment {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb;
} __attribute__((packed)) tss_t;

typedef struct cos_gdt_ptr {
    uint16_t gdt_size;
    uint64_t gdt_ptr;
} __attribute__((packed)) cos_gdt_ptr_t;
