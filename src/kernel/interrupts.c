////////////////////////
//
//  Created: Thu Jul 04 2024
//  File: interrupts.c
//
////////////////////////

#include "kernel/graphics/screen.h"
#include "kernel/idt.h"
#include "kernel/term/tty.h"
#include "kernel/interrupts.h"
#include "kernel/asm.h"
#include "kernel/vga.h"
#include "kernel/term/tty.h"
#include <stdint.h>
#include <stdnoreturn.h>
#include "kernel/cos_memory.h"

uint64_t handle_unix_timestamp(uint64_t new_timestamp)
{
    static uint64_t unix_timestamp = 0;

    if (new_timestamp)
        unix_timestamp = new_timestamp;
    return unix_timestamp;
}

void cos_common_handler(uint64_t num)
{
    const char interrupt[6] = {'0', 'x',
                                (char)(((num >> 4) & 0xF) > 0x9 ? ((num >> 4) & 0xF) + 55 : ((num >> 4) & 0xF) + 48),
                                (char)((num & 0xF) > 0x9 ? (num & 0xF) + 55 : (num & 0xF) + 48),
                                '\n', '\0'
    };

    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: RESERVED\n");
    cos_printf(interrupt);
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

//In practice, syscalls may have an infinite number of arguments.
//Without better defined process handlers, we can't yet retrieve all function arguments in an elegant way.
//TODO: Once processes are defined, implement user context switching instead via the syscall instruction.
void cos_syscall_handler(cos_cpu_stack *stack)
{
    switch (stack->rax) {
        case (1): cos_write_handler(stack->rdi, (char *)(stack->rsi), stack->rdx);
            break;
        case (60): cos_exit_handler(stack->rdi, &stack);
            break;
    }
}

void cos_division_by_zero_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: division by zero\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_single_step_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: single step\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_nmi_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: non-maskable interrupt\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_breakpoint_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: breakpoint\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_overflow_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: overflow\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_bound_range_exceeded_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: bound range exceeded\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_bound_invalid_opcode_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: bound invalid opcode\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_coprocessor_not_available_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: coprocessor not available\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_double_fault_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: double fault (Kernel bug)\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_segment_overrun_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: segment overrun\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_invalid_tss_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: invalid tss\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_segment_not_present_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: void segment not present\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_stack_segment_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: stack segment\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_general_protection_fault_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: general protection fault\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_page_fault_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: page fault\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_floating_point_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: x87 floating point exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_alignment_check_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: alignment check exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_machine_check_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: machine check exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_simd_floating_point_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: simd floating point exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_virtualization_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: virtualization exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_control_protection_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: control protection exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}


void cos_hypervisor_injection_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: hypervisor injection exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_vmm_communication_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: vmm communication exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_security_exception_handler(void)
{
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_RED, COS_VGA_COLOR_BLACK));
    cos_printf("INTERRUPT: security exception\n");
    cos_term_set_color(COS_VGA_ENTRY_COLOR(COS_VGA_COLOR_LIGHT_GREY, COS_VGA_COLOR_BLACK));
}

void cos_irq_master_handler(void)
{
    cos_outb(0x20, 0x20);
}

void cos_irq_slave_handler(void)
{
    cos_outb(0x20, 0x20);
    cos_outb(0xA0, 0x20);
}

void cos_irq_spurious_7_handler(void)
{
    cos_outb(0x20, 0x0A);
    cos_outb(0xA0, 0x0A);
    if (((cos_inb(0xA0) << 8) | cos_inb(0x20)) == 0b100000100)
        cos_outb(0x20, 0x20);
}

void cos_irq_spurious_15_handler(void)
{
    cos_outb(0x20, 0x0B);
    cos_outb(0xA0, 0x0B);
    if (((cos_inb(0xA0) << 8) | cos_inb(0x20)) == 0b1000000000000100)
        cos_outb(0xA0, 0x20);
    cos_outb(0x20, 0x20);
}

static uint8_t bcd_to_binary(uint8_t value)
{
    return ((value & 0xF0) >> 1) + ((value & 0xF0) >> 3) + (value & 0xF);
}

/*
    Very unoptimized algorithm.
    Epoch will always be January 1, 1970.
    The Weekday register (0x06) is unreliable and it should be determined by the date itself.
    The century register (0x32) may not exist as well if the hardware is too old (before 2014).
*/
void cos_irq_cmos_handler(void)
{
    uint8_t rtc_settings;
    uint64_t month_table[12] = {2678400, 5097600, 7776000, 10368000, 13046400, 15638400, 18316800, 20995200, 23587200, 26265600, 28857600, 31536000}; /*TO BE MADE BETTER*/
    uint64_t seconds = 0x00;
    uint64_t minutes = 0x00;
    uint64_t hours = 0x00;
    uint64_t month_day = 0x00;
    uint64_t month = 0x00;
    uint64_t leap_days = 8;
    uint64_t year = 0;     /*From Epoch until the 21rst century - 1 year.
                             Takes into account the fact that the year isn't yet over.*/
    cos_outb(0x70, 0x8B);
    rtc_settings = cos_inb(0x71);
/*
    if (rtc_settings & 0x10)
        cos_printf("RTC: 24 hour format set.\n");
    else
        cos_printf("RTC: 12 hour format set.\n");
    if (rtc_settings & 0x100)
        cos_printf("RTC: Binary Mode set.\n");
    else
        cos_printf("RTC: BCD Mode set.\n");
*/
    cos_outb(0x70, 0x80);
    seconds = !(rtc_settings & 0x100) ? bcd_to_binary(cos_inb(0x71)) : cos_inb(0x71);

    cos_outb(0x70, 0x82);
    minutes = !(rtc_settings & 0x100) ? bcd_to_binary(cos_inb(0x71)) : cos_inb(0x71);

    cos_outb(0x70, 0x84);
    hours = !(rtc_settings & 0x100) ? bcd_to_binary(cos_inb(0x71)) : cos_inb(0x71);
    hours = !(rtc_settings & 0x10) ? (hours & 0x80) ? (12 + (hours & 0x7F)) % 24 : hours : hours;

    cos_outb(0x70, 0x87);
    month_day = !(rtc_settings & 0x100) ? bcd_to_binary(cos_inb(0x71)) : cos_inb(0x71);

    cos_outb(0x70, 0x89);
    year += !(rtc_settings & 0x100) ? bcd_to_binary(cos_inb(0x71)) : cos_inb(0x71);

    cos_outb(0x70, 0x88);
    month = !(rtc_settings & 0x100) ? bcd_to_binary(cos_inb(0x71)) : cos_inb(0x71);
    leap_days += (month >= 3) && !(year & 0x11) ? (year >> 2) : (year >> 2) - 1;
    month = !(month - 1) ? 0 : month_table[month - 2];

    handle_unix_timestamp((year + 30) * 31536000 + month + (leap_days + month_day) * 86400 + hours * 3600 + minutes * 60 + seconds);

    //This sequence acknowledges reception of IRQ8.
    //If Register C isn't read, the IRQ won't be sent again.
    cos_outb(0x70, 0x8C);
    cos_inb(0x71);

    cos_outb(0x20, 0x20);
    cos_outb(0xA0, 0x20);
}

void cos_keyboard_handler(void)
{
    static const char scancode_to_ascii[128] = {
        0,   27, '&', 'e', '"', '\'', '(', '-', 'e', '_', /* 0x00 - 0x09 */
        'c', 'a', ')', '=', '\b', /* Backspace */
        '\t', /* Tab */
        'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', /* 0x10 - 0x19 */
        '^', '$', '\n', /* Enter */
        0,   /* Control */
        'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', /* 0x1E - 0x27 */
        'u', '*', 0, /* Left Shift */
        '<', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', /* 0x2C - 0x35 */
        '!', 0,   /* Right Shift */
        '*', 0,   /* Alt */
        ' ', /* Spacebar */
        0,   /* Caps Lock */
        0,   /* F1 */
        0,   /* F2 */
        0,   /* F3 */
        0,   /* F4 */
        0,   /* F5 */
        0,   /* F6 */
        0,   /* F7 */
        0,   /* F8 */
        0,   /* F9 */
        0,   /* F10 */
        0,   /* Num Lock */
        0,   /* Scroll Lock */
        0,   /* Home */
        0,   /* Up */
        0,   /* Page Up */
        '-',
        0,   /* Left */
        0,
        0,   /* Right */
        '+',
        0,   /* End */
        0,   /* Down */
        0,   /* Page Down */
        0,   /* Insert */
        0,   /* Delete */
        0, 0, 0, 0, /* F11 - F12 */
        0 /* All other keys */
    };
    static const int scancode_to_ascii_shift[128] = {
        0,   27, '1', '2', '3', '4', '5', '6', '7', '8', /* 0x00 - 0x09 */
        '9', '0', L'°', '+', '\b', /* Backspace */
        '\t', /* Tab */
        'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', /* 0x10 - 0x19 */
        L'¨', '*', '\n', /* Enter */
        0,   /* Control */
        'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', /* 0x1E - 0x27 */
        '%', L'µ', 0, /* Left Shift */
        '>', 'W', 'X', 'C', 'V', 'B', 'N', '?', '.', '/', /* 0x2C - 0x35 */
        L'§', 0,   /* Right Shift */
        '*', 0,   /* Alt */
        ' ', /* Spacebar */
        0,   /* Caps Lock */
        0,   /* F1 */
        0,   /* F2 */
        0,   /* F3 */
        0,   /* F4 */
        0,   /* F5 */
        0,   /* F6 */
        0,   /* F7 */
        0,   /* F8 */
        0,   /* F9 */
        0,   /* F10 */
        0,   /* Num Lock */
        0,   /* Scroll Lock */
        0,   /* Home */
        0,   /* Up */
        0,   /* Page Up */
        '-', /* Minus */
        0,   /* Left */
        0,
        0,   /* Right */
        '+', /* Plus */
        0,   /* End */
        0,   /* Down */
        0,   /* Page Down */
        0,   /* Insert */
        0,   /* Delete */
        0, 0, 0, 0, /* F11 - F12 */
        0 /* All other keys */
    };
    static cos_keyboard_state keyboard_state = {0};

    uint8_t scancode = cos_inb(KEYBOARD_DATA_PORT);

    // Left and right shift
    if (scancode == 42 || scancode == 54)
        keyboard_state.shift = true;

    if (!(scancode & 0x80)) {
        if (keyboard_state.shift)
            cos_term_input_char(scancode_to_ascii_shift[scancode]);
        else
            cos_term_input_char(scancode_to_ascii[scancode]);
    }
    else {
        scancode -= 0x80;

        // Left and right shift
        if (scancode == 42 || scancode == 54)
            keyboard_state.shift = false;
    }

    cos_outb(0X20, 0x20);
}
