////////////////////////
//
//  Created: Sun Jun 30 2024
//  File: idt.c
//
////////////////////////

#include "kernel/idt.h"
#include "kernel/interrupts.h"
#include "kernel/term/tty.h"
#include "kernel/asm.h"
#include <stdint.h>

static cos_interrupt_ptr COS_INTERRUPT_TABLE[] = {
    cos_division_by_zero_handler,
    cos_single_step_handler,
    cos_nmi_handler,
    cos_breakpoint_handler,
    cos_overflow_handler,
    cos_bound_range_exceeded_handler,
    cos_bound_invalid_opcode_handler,
    cos_coprocessor_not_available_handler,
    cos_double_fault_handler,
    cos_segment_overrun_handler,
    cos_invalid_tss_handler,
    cos_segment_not_present_handler,
    cos_stack_segment_handler,
    cos_general_protection_fault_handler,
    cos_page_fault_handler,
    NULL,
    cos_floating_point_exception_handler,
    cos_alignment_check_exception_handler,
    cos_machine_check_exception_handler,
    cos_simd_floating_point_exception_handler,
    cos_virtualization_exception_handler,
    cos_control_protection_exception_handler,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    cos_hypervisor_injection_exception_handler,
    cos_vmm_communication_exception_handler,
    cos_security_exception_handler,
    NULL,
    NULL,
    cos_keyboard_handler,
    cos_irq_master_handler,
    cos_irq_master_handler,
    cos_irq_master_handler,
    cos_irq_master_handler,
    cos_irq_master_handler,
    cos_irq_spurious_7_handler,
    cos_irq_cmos_handler,
    cos_irq_slave_handler,
    cos_irq_slave_handler,
    cos_irq_slave_handler,
    cos_irq_slave_handler,
    cos_irq_slave_handler,
    cos_irq_slave_handler,
    cos_irq_spurious_15_handler,
    cos_syscall80
};

static cos_idt_entry cos_idt[IDT_SIZE];
static cos_idt_ptr cos_idtp;

void cos_idt_load(void)
{
    asm volatile("lidt (%0)" : : "r"(&cos_idtp));
}

void cos_idt_set_gate(uint8_t num, uint64_t base, uint8_t gate_type)
{
    cos_idt[num].offset_1 = base & 0xFFFF;
    cos_idt[num].offset_2 = (base >> 16) & 0xFFFF;
    cos_idt[num].offset_3 = (base >> 32) & 0xFFFFFFFF;
    cos_idt[num].selector = 0x08;
    cos_idt[num].ist = 0;
    cos_idt[num].type_attributes = gate_type;
    cos_idt[num].zero = 0;
}

cos_cpu_stack *cos_resolve_interrupt(cos_cpu_stack *stack)
{
    static size_t table_size = sizeof(COS_INTERRUPT_TABLE) / sizeof(cos_interrupt_ptr);

    if (stack->interrupt_num == 0x80)
        cos_syscall_handler(stack);
    else if (stack->interrupt_num == 0x20)
        cos_scheduling_handler(stack);
    else if (stack->interrupt_num == 0x21)
        cos_keyboard_handler();
    else if (stack->interrupt_num == 0x28)
        cos_irq_cmos_handler();
    else if (stack->interrupt_num == 0x2e)
        cos_irq_slave_handler();
    else if (stack->interrupt_num >= table_size || !COS_INTERRUPT_TABLE[stack->interrupt_num])
        cos_common_handler(stack->interrupt_num);
    else
        COS_INTERRUPT_TABLE[stack->interrupt_num]();

    return stack;
}

void cos_idt_init(void)
{
    cos_idtp.limit = sizeof(cos_idt_entry) * IDT_SIZE -1;
    cos_idtp.base = (uint64_t)&cos_idt;

    for (size_t i = 0; i < IDT_SIZE; ++i) {
        cos_idt[i].offset_1 = 0;
        cos_idt[i].offset_2 = 0;
        cos_idt[i].offset_3 = 0;
        cos_idt[i].selector = 0;
        cos_idt[i].ist = 0;
        cos_idt[i].type_attributes = 0;
        cos_idt[i].zero = 0;
    }

    cos_idt_set_gate(COS_INTERRUPT_DIVISION_BY_ZERO_NUM, (uint64_t)cos_isr0_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_SINGLE_STEP_NUM, (uint64_t)cos_isr1_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_NMI_NUM, (uint64_t)cos_isr2_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_BREAK_POINT_NUM, (uint64_t)cos_isr3_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_OVERFLOW_NUM, (uint64_t)cos_isr4_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_BOUND_RANGE_EXCEEDED_NUM, (uint64_t)cos_isr5_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_INVALID_OPCODE_NUM, (uint64_t)cos_isr6_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_COPROCESSOR_NOT_AVAILABLE_NUM, (uint64_t)cos_isr7_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_DOUBLE_FAULT_NUM, (uint64_t)cos_isr8_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_SEGMENT_OVERRUN_NUM, (uint64_t)cos_isr9_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_INVALID_TSS_NUM, (uint64_t)cos_isr10_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_SEGMENT_NOT_PRESENT_NUM, (uint64_t)cos_isr11_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_STACK_SEGMENT_FAULT_NUM, (uint64_t)cos_isr12_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_GENERAL_PROTECTION_FAULT_NUM, (uint64_t)cos_isr13_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_PAGE_FAULT_NUM, (uint64_t)cos_isr14_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_15_NUM, (uint64_t)cos_isr15_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_x87_FLOATING_POINT_EXCEPTION_NUM, (uint64_t)cos_isr16_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_ALIGNMENT_CHECK_NUM, (uint64_t)cos_isr17_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_MACHINE_CHECK_NUM, (uint64_t)cos_isr18_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_SIMD_FLOATING_POINT_EXCEPTION_NUM, (uint64_t)cos_isr19_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_VIRTUALIZATION_EXCEPTION_NUM, (uint64_t)cos_isr20_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_CONTROL_PROTECTION_EXCEPTION_NUM, (uint64_t)cos_isr21_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_22_NUM, (uint64_t)cos_isr22_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_23_NUM, (uint64_t)cos_isr23_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_24_NUM, (uint64_t)cos_isr24_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_25_NUM, (uint64_t)cos_isr25_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_26_NUM, (uint64_t)cos_isr26_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_27_NUM, (uint64_t)cos_isr27_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_HYPERVISOR_INJECTION_EXCEPTION_NUM, (uint64_t)cos_isr28_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_VMM_COMMUNICATION_EXCEPTION_NUM, (uint64_t)cos_isr29_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_SECURITY_EXCEPTION_NUM, (uint64_t)cos_isr30_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_RESERVED_31_NUM, (uint64_t)cos_isr31_handler, 0x8F);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_0, (uint64_t)cos_irq0_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_KEYBOARD_NUM, (uint64_t)cos_irq1_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_2, (uint64_t)cos_irq2_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_3, (uint64_t)cos_irq3_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_4, (uint64_t)cos_irq4_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_5, (uint64_t)cos_irq5_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_6, (uint64_t)cos_irq6_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_7, (uint64_t)cos_irq7_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_8, (uint64_t)cos_irq8_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_9, (uint64_t)cos_irq9_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_10, (uint64_t)cos_irq10_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_11, (uint64_t)cos_irq11_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_12, (uint64_t)cos_irq12_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_13, (uint64_t)cos_irq13_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_14, (uint64_t)cos_irq14_handler, 0x8E);
    cos_idt_set_gate(COS_INTERRUPT_REQUEST_15, (uint64_t)cos_irq15_handler, 0x8E);
    cos_idt_set_gate(0x80, (uint64_t)cos_syscall80, 0x8E);

    for (int i = 9; i < 32; ++i)
        cos_idt_set_gate(i, (uint64_t)cos_default_handler, 0x8F);

    for (int i = 48; i < 256; ++i) {
        if (i == 0x80)
            continue;
        cos_idt_set_gate(i, (uint64_t)cos_default_handler, 0x8E);
    }

    cos_idt_load();
    asm volatile("sti");
}
