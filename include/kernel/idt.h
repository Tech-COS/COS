////////////////////////
//
//  Created: Sun Jun 30 2024
//  File: idt.h
//
////////////////////////

#pragma once

#include "types.h"
#include <stdint.h>

#define IDT_SIZE 256

typedef struct cos_idt_entry {
   uint16_t offset_1;
   uint16_t selector;
   uint8_t  ist;
   uint8_t  type_attributes;
   uint16_t offset_2;
   uint32_t offset_3;
   uint32_t zero;
} __attribute__((packed)) cos_idt_entry;

typedef struct cos_idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) cos_idt_ptr;

void cos_idt_load(void);
void cos_idt_set_gate(uint8_t num, uint64_t base, uint8_t gate_type);
void cos_idt_init(void);

typedef struct cos_cpu_stack {
    //Registers in reverse order of the ones pushed to the stack.
    // Happens because the addresses grow down as they are pushed to it.
    //uint64_t cr4;
    uint64_t cr3;
    //uint64_t cr2;
    //uint64_t cr0;
    uint64_t gs;
    uint64_t fs;
    uint64_t rbp;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    //What was pushed in all handlers for proper alignment
    uint64_t interrupt_num;
    uint64_t error;
    //On interrupt, these registers are preserved by the cpu and will be returned to their
    //original states after an iretq instruction is reached.
    uint64_t iret_frame_rip;
    uint64_t iret_frame_cs;
    uint64_t iret_frame_flags;
    uint64_t iret_frame_rsp;
    uint64_t iret_frame_ss;
} __attribute__((packed)) cos_cpu_stack;
