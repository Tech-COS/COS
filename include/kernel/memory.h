////////////////////////
//
//  Created: Sun Oct 06 2024
//  File: memory.h
//
////////////////////////

#pragma once

#define COS_PAGE_TABLE_ENTRY_COUNT 512
#define COS_KERNEL_BASE_ADDRESS 0xFFFFFF8000000000

typedef struct cos_page_table_entry {
    unsigned long present: 1;               // the page is currently in memory
    unsigned long writable: 1;              // it’s allowed to write to this page
    unsigned long user_accessible: 1;       // if not set, only kernel mode code can access this page
    unsigned long write_through_caching: 1; // writes go directly to memory
    unsigned long disable_cache: 1;         // no cache is used for this page
    unsigned long accessed: 1;              // the CPU sets this bit when this page is used
    unsigned long dirty: 1;                 // the CPU sets this bit when a write to this page occurs
    unsigned long huge_page: 1;             // must be 0 in P1 and P4, creates a 1 GiB page in P3, creates a 2 MiB page in P2
    unsigned long global: 1;                // page isn’t flushed from caches on address space switch (PGE bit of CR4 register must be set)
    unsigned long available1: 3;            // can be used freely by the OS
    unsigned long physical_address: 40;     // the page aligned 52bit physical address of the frame or the next page table
    unsigned long available2: 10;           // can be used freely by the OS
    unsigned long no_execute: 1;            // forbid executing code on this page (the NXE bit in the EFER register must be set)
} cos_page_table_entry;

typedef struct cos_page_table {
    cos_page_table_entry entries[COS_PAGE_TABLE_ENTRY_COUNT];
} cos_page_table;
