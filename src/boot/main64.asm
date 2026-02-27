bits 64
global long_mode_start
global tss_stack_top
global reload_segments
extern cos_kernel_main
extern setup_gdt
extern gdt64
extern gdt64_ptr
extern page_table_level_4
extern enable_syscall

section .text

long_mode_start:
    cli
    call initialise_segments
    call initialise_pic
    call initialise_rtc

    mov rax, load_kernel_in_higher_half
    jmp rax                             ; By jumping to the load_kernel_in_higher_half label,
                                        ; we force the rip pointer to point to virtual memory instead of physical memory.
                                        ; An indirect jump is made because we must pass the higher half label address to it.
                                        ; Otherwise, jmp would take the physical address instead.

; In 64bit mode, the only instruction supporting 64bit immediate addresses is MOV (MOVABS in AT&T format).
; 64bit operations still work for all instructions but only with registers.
load_kernel_in_higher_half:

; Resets the stack pointer to the new kernel base.
    mov rax, 0xFFFFFF8000000000
    add rsp, rax

; Unmapping of the first entry of the Page Global Directory, requires reloading it into Control Register 3.
; This is done because the virtual addresses from 0x0 to 0x200000 are still identity mapped.
; After this operation, only the last entry remains and maps physical addresses 0x0 - 0x200000 to virtual addresses 0xFFFFFF8000000000 - 0xFFFFFF8000200000.
    xor rax, rax
    mov [qword page_table_level_4], rax
    mov rax, cr3
    mov cr3, rax
; The instruction invplg may not be called directly by the kernel because reloading the page table entries in cr3 will flush all pages from the TLB.

; The Global Descriptor Table has been unmapped from virtual memory but the processor still uses the old address.
; It is reloaded again with the new kernel base.

    push rdi
    call setup_gdt
    pop rdi

    call enable_sse
    push rdi
    call enable_syscall
    pop rdi

; The multiboot structure passed to edi during initialisation doesn't point to the new kernel base.
; It is updated this way.
    mov rbx, 0xFFFFFF8000000000
	add rbx, rdi
    mov rdi, rbx

    call cos_kernel_main

    ; In case kernel mode is returned unexpectedly, the cpu must be halted.
    .loop:
        hlt
        jmp .loop

; In 64bit mode, far jumps don't exist.
; After jumping to higher half, the segment registers must be reloaded with the new base addresses.
; As CS can't be updated directly, a far return is used instead of a far jump.
; The RETFQ instruction will take two values from the stack and update the CS and RID registers accordingly.
reload_segments:
    mov rax, initialise_segments
    push 0x8
    push rax
    retfq

; Initialization of data segments is done correctly even if they aren't used at all except for fs and gs in 64bit mode.
initialise_segments:
    mov rax, 0x10
    mov ss, rax
    mov ds, rax
    mov es, rax
    mov fs, rax
    mov gs, rax
    ret

; The PIC which is shorthand for Programmable Interrupt Controller handles CPU exceptions and interrupts.
; It must be properly initialised before handling kernel code because it will trigger exceptions otherwise.
; TODO: Must be deprecated and changed to APIC support later down the line.

; By default, the PIC possesses 2 parts but can include more depending on processors.
; The Master PIC is communicated with using the command port 0x20 and the data port 0x21.
; The Slave PIC is communicated with using the command port 0xA0 and the data port 0xA1.
; Each of them controls IRQs ranging from 0-7 and 8-15 respectively.
; We remap them because they use the first 16 entries of the interrupt descriptor table (IDT) by default (because it doesn't overlap with exceptions in real mode).
; As such, they therefore overlap with the interrupt vectors of the exceptions for protected mode.
initialise_pic:

    ; Initialisation control word 1 (ICW1)
	mov al, 0x11 ; Bit 1 set means ICW4 is expected during initialisation. Bit 4 set means initialisation will be made.
	out 0x20, al ; Sends the initialise command (0x10) to Master PIC and makes it wait for additional information (0x01)
	out 0xA0, al ; Sends the initialise command (0x10) to Slave PIC and makes it wait for additional information (0x01)

    ; ICW2 (In 80x86 mode, it defines the interrupt vector addresses).
	mov al, 0x20 ; Sets IRQ0 to the 33rd entry of the IDT (It automatically sets up IRQ1-7 to subsequent entries).
	out 0x21, al
	mov al, 0x28 ; Sets IRQ8 to the 41rst entry of the IDT (It automatically sets up IRQ9-15 to subsequent entries).
	out 0xA1, al

    ; ICW3
	mov al, 4   ; Tells Master PIC that there's a slave PIC at IRQ2 (It uses bit notation to determine the IRQ Line. 4 is 0b100 in binary, which means the second bit is flipped. So, it is IRQ2).
	out 0x21, al; IRQ2 was chosen because the x86 architecture uses IRQ2 for communication between the master and the slave.
	mov al, 2   ; Tells Slave PIC its cascade identity. Only bit 0 to 2 are used to communicate.
	out 0xA1, al ; To pass the IR line that the master must communicate on, 3bit value ranging from 0 to 7 must be passed. Since IR line 2 is used for communication, we send 2 to the master.

    ; ICW4
	mov al, 1    ; Bit 0 enables 8086 mode.
	out 0x21, al ; By default, it works in 8080 mode which sends 3 bytes of data to the data bus equivalent to a call instruction in 8080 processors.
	out 0xA1, al ; In 8086 mode, it sends an interrupt number.

	ret

initialise_rtc:

    mov al, 0x8B
    out 0x70, al
    in al, 0x71

    mov dl, al
    or dl, 0x40

    mov al, 0x8B
    out 0x70, al
    mov al, dl
    out 0x71, al

    ret

enable_sse:
    push rax
    push rcx
    push rdx

    mov eax, 0x1
    cpuid
    cmp edx, 0b11100001 << 19 ; Bit 26 and 25 are set to 1 if SSE is available. fxsave/fxstor can save SSE state if bit 24 is set to 1. clflush is supported if bit 19 is available.
    jz error
    cmp ecx, 0b1000000001 ; Bit 9 and 0 are set to 1 if support for SSE is available.
    jz error

    mov rax, cr4
    or eax, 515 << 9		; Sets CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, rax

    pop rdx
    pop rcx
    pop rax
    ret

error:									; Prints the Error.
	mov dword [0xb8000], 0x4f524f45
	mov dword [0xb8004], 0x4f3a4f52
	mov dword [0xb8008], 0x4f204f20

; The CPU will remain in this state indefinitely until it is rebooted.
loop_halt_cpu:
	hlt
	jmp loop_halt_cpu

section .bss

align 16
tss_stack_bottom:
	resb 65536									; The resb command declares unitialised space for operations.
tss_stack_top:
