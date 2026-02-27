////////////////////////
//
//  Created: Thu Jun 27 2024
//  File: asm.h
//
////////////////////////

#pragma once

#include <stdint.h>

extern void cos_outb(uint16_t port, uint8_t value);
extern uint8_t cos_inb(uint16_t port);
extern void cos_out16(uint16_t port, uint16_t value);
extern uint16_t cos_in16(uint16_t port);
extern void cos_out32(uint16_t port, uint32_t value);
extern uint32_t cos_in32(uint16_t port);
