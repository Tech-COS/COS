////////////////////////
//
//  Created: Thu Jul 04 2024
//  File: interrupts.h
//
////////////////////////

#pragma once

#include "types.h"
#include "idt.h"
#include <stdint.h>

typedef struct cos_keyboard_state {
    bool shift;
} cos_keyboard_state;

#define KEYBOARD_DATA_PORT 0x60

#define COS_INTERRUPT_DIVISION_BY_ZERO_NUM 0
#define COS_INTERRUPT_SINGLE_STEP_NUM 1
#define COS_INTERRUPT_NMI_NUM 2
#define COS_INTERRUPT_BREAK_POINT_NUM 3
#define COS_INTERRUPT_OVERFLOW_NUM 4
#define COS_INTERRUPT_BOUND_RANGE_EXCEEDED_NUM 5
#define COS_INTERRUPT_INVALID_OPCODE_NUM 6
#define COS_INTERRUPT_COPROCESSOR_NOT_AVAILABLE_NUM 7
#define COS_INTERRUPT_DOUBLE_FAULT_NUM 8
#define COS_INTERRUPT_SEGMENT_OVERRUN_NUM 9
#define COS_INTERRUPT_INVALID_TSS_NUM 10
#define COS_INTERRUPT_SEGMENT_NOT_PRESENT_NUM 11
#define COS_INTERRUPT_STACK_SEGMENT_FAULT_NUM 12
#define COS_INTERRUPT_GENERAL_PROTECTION_FAULT_NUM 13
#define COS_INTERRUPT_PAGE_FAULT_NUM 14
#define COS_INTERRUPT_RESERVED_15_NUM 15
#define COS_INTERRUPT_x87_FLOATING_POINT_EXCEPTION_NUM 16
#define COS_INTERRUPT_ALIGNMENT_CHECK_NUM 17
#define COS_INTERRUPT_MACHINE_CHECK_NUM 18
#define COS_INTERRUPT_SIMD_FLOATING_POINT_EXCEPTION_NUM 19
#define COS_INTERRUPT_VIRTUALIZATION_EXCEPTION_NUM 20
#define COS_INTERRUPT_CONTROL_PROTECTION_EXCEPTION_NUM 21
#define COS_INTERRUPT_RESERVED_22_NUM 22
#define COS_INTERRUPT_RESERVED_23_NUM 23
#define COS_INTERRUPT_RESERVED_24_NUM 24
#define COS_INTERRUPT_RESERVED_25_NUM 25
#define COS_INTERRUPT_RESERVED_26_NUM 26
#define COS_INTERRUPT_RESERVED_27_NUM 27
#define COS_INTERRUPT_HYPERVISOR_INJECTION_EXCEPTION_NUM 28
#define COS_INTERRUPT_VMM_COMMUNICATION_EXCEPTION_NUM 29
#define COS_INTERRUPT_SECURITY_EXCEPTION_NUM 30
#define COS_INTERRUPT_RESERVED_31_NUM 31
#define COS_INTERRUPT_REQUEST_0 32
#define COS_INTERRUPT_KEYBOARD_NUM 33
#define COS_INTERRUPT_REQUEST_2 34
#define COS_INTERRUPT_REQUEST_3 35
#define COS_INTERRUPT_REQUEST_4 36
#define COS_INTERRUPT_REQUEST_5 37
#define COS_INTERRUPT_REQUEST_6 38
#define COS_INTERRUPT_REQUEST_7 39
#define COS_INTERRUPT_REQUEST_8 40
#define COS_INTERRUPT_REQUEST_9 41
#define COS_INTERRUPT_REQUEST_10 42
#define COS_INTERRUPT_REQUEST_11 43
#define COS_INTERRUPT_REQUEST_12 44
#define COS_INTERRUPT_REQUEST_13 45
#define COS_INTERRUPT_REQUEST_14 46
#define COS_INTERRUPT_REQUEST_15 47

extern void cos_isr0_handler(void);
extern void cos_isr1_handler(void);
extern void cos_isr2_handler(void);
extern void cos_isr3_handler(void);
extern void cos_isr4_handler(void);
extern void cos_isr5_handler(void);
extern void cos_isr6_handler(void);
extern void cos_isr7_handler(void);
extern void cos_isr8_handler(void);
extern void cos_isr9_handler(void);
extern void cos_isr10_handler(void);
extern void cos_isr11_handler(void);
extern void cos_isr12_handler(void);
extern void cos_isr13_handler(void);
extern void cos_isr14_handler(void);
extern void cos_isr15_handler(void);
extern void cos_isr16_handler(void);
extern void cos_isr17_handler(void);
extern void cos_isr18_handler(void);
extern void cos_isr19_handler(void);
extern void cos_isr20_handler(void);
extern void cos_isr21_handler(void);
extern void cos_isr22_handler(void);
extern void cos_isr23_handler(void);
extern void cos_isr24_handler(void);
extern void cos_isr25_handler(void);
extern void cos_isr26_handler(void);
extern void cos_isr27_handler(void);
extern void cos_isr28_handler(void);
extern void cos_isr29_handler(void);
extern void cos_isr30_handler(void);
extern void cos_isr31_handler(void);
extern void cos_irq0_handler(void);
extern void cos_irq1_handler(void);
extern void cos_irq2_handler(void);
extern void cos_irq3_handler(void);
extern void cos_irq4_handler(void);
extern void cos_irq5_handler(void);
extern void cos_irq6_handler(void);
extern void cos_irq7_handler(void);
extern void cos_irq8_handler(void);
extern void cos_irq9_handler(void);
extern void cos_irq10_handler(void);
extern void cos_irq11_handler(void);
extern void cos_irq12_handler(void);
extern void cos_irq13_handler(void);
extern void cos_irq14_handler(void);
extern void cos_irq15_handler(void);
extern void cos_default_handler(void);
extern void cos_syscall80(void);
extern void cos_scheduling_handler(cos_cpu_stack *stack);

void cos_syscall_handler(cos_cpu_stack *stack);
void cos_common_handler(uint64_t num);
void cos_division_by_zero_handler(void);
void cos_single_step_handler(void);
void cos_nmi_handler(void);
void cos_breakpoint_handler(void);
void cos_overflow_handler(void);
void cos_bound_range_exceeded_handler(void);
void cos_bound_invalid_opcode_handler(void);
void cos_coprocessor_not_available_handler(void);
void cos_double_fault_handler(void);
void cos_segment_overrun_handler(void);
void cos_invalid_tss_handler(void);
void cos_segment_not_present_handler(void);
void cos_stack_segment_handler(void);
void cos_general_protection_fault_handler(void);
void cos_page_fault_handler(void);
void cos_floating_point_exception_handler(void);
void cos_alignment_check_exception_handler(void);
void cos_machine_check_exception_handler(void);
void cos_simd_floating_point_exception_handler(void);
void cos_virtualization_exception_handler(void);
void cos_control_protection_exception_handler(void);
void cos_hypervisor_injection_exception_handler(void);
void cos_vmm_communication_exception_handler(void);
void cos_security_exception_handler(void);

void cos_irq_master_handler(void);
void cos_irq_slave_handler(void);
void cos_irq_spurious_7_handler(void);
void cos_irq_spurious_15_handler(void);
void cos_keyboard_handler(void);
void cos_irq_cmos_handler(void);
uint64_t handle_unix_timestamp(uint64_t new_timestamp);
