////////////////////////
//
//  Created: Thu Jun 27 2024
//  File: asm.c
//
////////////////////////

#include "kernel/asm.h"

void cos_outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1" ::"a"(value), "Nd"(port) : "memory");
}

uint8_t cos_inb(uint16_t port)
{
    uint8_t data;
    asm volatile("inb %1, %0" : "=r"(data) : "dN"(port) : "memory");
    return data;
}

void cos_out16(uint16_t port, uint16_t value)
{
    asm volatile("outw %0, %1" ::"a"(value), "Nd"(port) : "memory");
}

uint16_t cos_in16(uint16_t port)
{
    uint16_t data;
    asm volatile("inw %1, %0" : "=a"(data) : "dN"(port) : "memory");
    return data;
}


void cos_out32(uint16_t port, uint32_t value)
{
    asm volatile("outl %0, %1" ::"a"(value), "Nd"(port) : "memory");
}

uint32_t cos_in32(uint16_t port)
{
    uint32_t data;
    asm volatile("inl %1, %0" : "=a"(data) : "dN"(port) : "memory");
    return data;
}
