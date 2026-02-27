bits 64
global cos_isr0_handler
global cos_isr1_handler
global cos_isr2_handler
global cos_isr3_handler
global cos_isr4_handler
global cos_isr5_handler
global cos_isr6_handler
global cos_isr7_handler
global cos_isr8_handler
global cos_isr9_handler
global cos_isr10_handler
global cos_isr11_handler
global cos_isr12_handler
global cos_isr13_handler
global cos_isr14_handler
global cos_isr15_handler
global cos_isr16_handler
global cos_isr17_handler
global cos_isr18_handler
global cos_isr19_handler
global cos_isr20_handler
global cos_isr21_handler
global cos_isr22_handler
global cos_isr23_handler
global cos_isr24_handler
global cos_isr25_handler
global cos_isr26_handler
global cos_isr27_handler
global cos_isr28_handler
global cos_isr29_handler
global cos_isr30_handler
global cos_isr31_handler
global cos_irq0_handler
global cos_irq1_handler
global cos_irq2_handler
global cos_irq3_handler
global cos_irq4_handler
global cos_irq5_handler
global cos_irq6_handler
global cos_irq7_handler
global cos_irq8_handler
global cos_irq9_handler
global cos_irq10_handler
global cos_irq11_handler
global cos_irq12_handler
global cos_irq13_handler
global cos_irq14_handler
global cos_irq15_handler
global cos_default_handler
global cos_syscall80
global restore_preserved_state

extern x87_state ; Far from being the best way to do that.
extern cos_resolve_interrupt

section .text

align 16
; To use Interrupts properly, the environment must be saved and then returned to properly.
interrupt_resolver:
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rbp
    push fs
    push gs
    ;mov rdi, cr0
    ;push rdi
    ;mov rdi, cr2
    ;push rdi
    mov rdi, cr3
    push rdi
    ;mov rdi, cr4
    ;push rdi
    fxsave64 [rel x87_state]

    ; By using the calling convention of C, a pointer to the stack pointer is passed as the first argument of the called C function in rdi.
    ; A C structure is then used to retrieve the preserved information from the top of the stack to extract the handler number and the error code that were pushed to the stack.
    ; Upon return, the calling convention makes it so that rax possesses the return value of the function which is still the pointer to rsp.
    ; It is then returned to the preserved state before removing all the pushed registers and values before exiting, therefore returning the CPU to the state it was in before the interrupt occurred.
    mov rdi, rsp
    call cos_resolve_interrupt

    mov rsp, rax
    fxrstor64 [rel x87_state]
    ;pop rax
    ;mov cr4, rax
    pop rax
    mov cr3, rax
    ;pop rax
    ;mov cr2, rax
    ;pop rax
    ;mov cr0, rax
    pop gs
    pop fs
    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    cmp byte [rsp], 0x80
    je syscall_return_handler
    add rsp, 16 ; This removes 2 entries from the stack (in 64bit mode, each entry is 8 bytes long).
                ; They correspond to the two pushed values after entry to the irq handlers.
                ; The first is an error code if the handler has one and the other is the id of the handler.
    sti
    iretq       ; This is the standard Interrupt RETurn which pops the IRET Frame from the stack.
                ; The IRET Frame are 5 values preserved by the CPU which are pushed to the stack upon entry in the interrupt handler.
syscall_return_handler:
    add rsp, 16 ; This removes 2 entries from the stack (in 64bit mode, each entry is 8 bytes long).
                ; They correspond to the two pushed values after entry to the irq handlers.
                ; The first is an error code if the handler has one and the other is the id of the handler.
    sti
    o64 sysret

align 16
cos_syscall80:
    ;xchg bx, bx

    cli
    push 0
    push 0x80
    jmp interrupt_resolver

; By design, all handlers must be aligned called with an alignment of 16 bytes.
; Handlers are all set in C code and those that are reserved use are also defined in case the CPU acts incorrectly.
; By default, reserved handlers do nothing.
align 16
cos_isr0_handler:
    ;xchg bx, bx
    
    cli
    push 0
    push 0

    jmp interrupt_resolver

align 16
cos_isr1_handler:
    ;xchg bx, bx

    cli
    push 0
    push 1

    jmp interrupt_resolver

align 16
cos_isr2_handler:
    ;xchg bx, bx

    cli
    push 0
    push 2

    jmp interrupt_resolver

align 16
cos_isr3_handler:
    ;xchg bx, bx

    cli
    push 0
    push 3

    jmp interrupt_resolver

align 16
cos_isr4_handler:
    ;xchg bx, bx

    push 0
    push 4

    jmp interrupt_resolver

align 16
cos_isr5_handler:
    ;xchg bx, bx

    cli
    push 0
    push 5

    jmp interrupt_resolver

align 16
cos_isr6_handler:
    ;xchg bx, bx

    cli
    push 0
    push 6

    jmp interrupt_resolver

align 16
cos_isr7_handler:
    ;xchg bx, bx

    cli
    push 0
    push 7

    jmp interrupt_resolver

align 16
cos_isr8_handler:
    ;xchg bx, bx

    cli
    push 8

    jmp interrupt_resolver

align 16
cos_isr9_handler:
    ;xchg bx, bx

    cli
    push 0
    push 9

    jmp interrupt_resolver

align 16
cos_isr10_handler:
    ;xchg bx, bx

    cli
    push 10

    jmp interrupt_resolver

align 16
cos_isr11_handler:
    ;xchg bx, bx

    cli
    push 11

    jmp interrupt_resolver

align 16
cos_isr12_handler:
    ;xchg bx, bx

    cli
    push 12

    jmp interrupt_resolver

align 16
cos_isr13_handler:
    ;xchg bx, bx

    cli
    push 13

    jmp interrupt_resolver

align 16
cos_isr14_handler:
    ;xchg bx, bx

    cli
    push 14

    jmp interrupt_resolver

align 16
cos_isr15_handler:
    ;xchg bx, bx

    cli
    push 0
    push 15

    jmp interrupt_resolver

align 16
cos_isr16_handler:
    ;xchg bx, bx

    cli
    push 0
    push 16

    jmp interrupt_resolver

align 16
cos_isr17_handler:
    ;xchg bx, bx

    cli
    push 17

    jmp interrupt_resolver

align 16
cos_isr18_handler:
    ;xchg bx, bx

    cli
    push 0
    push 18

    jmp interrupt_resolver

align 16
cos_isr19_handler:
    ;xchg bx, bx

    cli
    push 0
    push 19

    jmp interrupt_resolver

align 16
cos_isr20_handler:
    ;xchg bx, bx

    cli
    push 0
    push 20

    jmp interrupt_resolver

align 16
cos_isr21_handler:
    ;xchg bx, bx

    cli
    push 21

    jmp interrupt_resolver

align 16
cos_isr22_handler:
    ;xchg bx, bx

    cli
    push 0
    push 22

    jmp interrupt_resolver

align 16
cos_isr23_handler:
    ;xchg bx, bx

    cli
    push 0
    push 23

    jmp interrupt_resolver

align 16
cos_isr24_handler:
    ;xchg bx, bx

    cli
    push 0
    push 24

    jmp interrupt_resolver

align 16
cos_isr25_handler:
    ;xchg bx, bx

    cli
    push 0
    push 25

    jmp interrupt_resolver

align 16
cos_isr26_handler:
    ;xchg bx, bx

    cli
    push 0
    push 26

    jmp interrupt_resolver

align 16
cos_isr27_handler:
    ;xchg bx, bx

    cli
    push 0
    push 27

    jmp interrupt_resolver

align 16
cos_isr28_handler:
    ;xchg bx, bx

    cli
    push 0
    push 28

    jmp interrupt_resolver

align 16
cos_isr29_handler:
    ;xchg bx, bx

    cli
    push 0
    push 29

    jmp interrupt_resolver

align 16
cos_isr30_handler:
    ;xchg bx, bx

    cli
    push 0
    push 30

    jmp interrupt_resolver

align 16
cos_isr31_handler:
    ;xchg bx, bx

    cli
    push 0
    push 31

    jmp interrupt_resolver

align 16
cos_irq0_handler:

    cli
    push 0
    push 32

    jmp interrupt_resolver

align 16
cos_irq1_handler:
    ;xchg bx, bx

    cli
    push 0
    push 33

    jmp interrupt_resolver

align 16
cos_irq2_handler:
    ;xchg bx, bx

    cli
    push 0
    push 34

    jmp interrupt_resolver

align 16
cos_irq3_handler:
    ;xchg bx, bx

    cli
    push 0
    push 35

    jmp interrupt_resolver

align 16
cos_irq4_handler:
    ;xchg bx, bx

    cli
    push 0
    push 36

    jmp interrupt_resolver

align 16
cos_irq5_handler:
    ;xchg bx, bx

    cli
    push 0
    push 37

    jmp interrupt_resolver

align 16
cos_irq6_handler:
    ;xchg bx, bx

    cli
    push 0
    push 38

    jmp interrupt_resolver

align 16
cos_irq7_handler:
    ;xchg bx, bx

    cli
    push 0
    push 39

    jmp interrupt_resolver

align 16
cos_irq8_handler:

    cli
    push 0
    push 40

    jmp interrupt_resolver

align 16
cos_irq9_handler:
    ;xchg bx, bx

    cli
    push 0
    push 41

    jmp interrupt_resolver

align 16
cos_irq10_handler:
    ;xchg bx, bx

    cli
    push 0
    push 42

    jmp interrupt_resolver

align 16
cos_irq11_handler:
    ;xchg bx, bx

    cli
    push 0
    push 43

    jmp interrupt_resolver

align 16
cos_irq12_handler:
    ;xchg bx, bx

    cli
    push 0
    push 44

    jmp interrupt_resolver

align 16
cos_irq13_handler:
    ;xchg bx, bx

    cli
    push 0
    push 45

    jmp interrupt_resolver

align 16
cos_irq14_handler:
    ;xchg bx, bx

    cli
    push 0
    push 46

    jmp interrupt_resolver

align 16
cos_irq15_handler:
    ;xchg bx, bx

    cli
    push 0
    push 47

    jmp interrupt_resolver

align 16
cos_default_handler:
    ;xchg bx, bx

    cli
    push 48

    jmp interrupt_resolver

