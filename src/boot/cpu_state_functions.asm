BITS 64

global backup_cpu_state
global windows_rearrange_arguments
global _cos_exit
global _cos_write
global x87_state

section .text

backup_cpu_state:

    mov [rdi + 120], rax
    push rax
    mov rax, cr4
    mov [rdi], rax
    mov rax, cr3
    mov [rdi + 8], rax
    mov rax, cr2
    mov [rdi + 16], rax
    mov rax, cr0
    mov [rdi + 24], rax

    mov [rdi + 32], gs
    mov [rdi + 40], fs

    mov [rdi + 48], rbp
    mov [rdi + 56], r15
    mov [rdi + 64], r14
    mov [rdi + 72], r13
    mov [rdi + 80], r12
    mov [rdi + 88], r11
    mov [rdi + 96], r10
    mov [rdi + 104], r9
    mov [rdi + 112], r8

    mov [rdi + 128], rbx
    mov [rdi + 136], rcx
    mov [rdi + 144], rdx
    mov [rdi + 152], rsi
    mov [rdi + 160], rdi

    mov [rdi + 192], cs
    mov [rdi + 208], rsp
    mov [rdi + 216], ss

    pushfq
    pop qword [rdi + 200]
    fxsave64 [rdi + 224]
    lea rax, [rel backup_cpu_state.return_from_backup_cpu_state]
    mov [rdi + 184], rax

.loop:
    dec rdx
    mov al, [byte rcx + rdx]
    mov [byte rsi + rdx], al

    test rdx, rdx
    jne .loop

.return_from_backup_cpu_state:
    pop rax
    ret

_cos_exit:
    mov rsp, rsi
	push	0x12345678
	push	0x12345678
	push	0x12345678
	push	0x12345678
	push	0x12345678
    mov rax, 60
    o64 syscall
    nop

_cos_write:
	push	r11
	push	rcx
    mov rax, 1
    o64 syscall
    pop rcx
    pop r11
    ret

windows_rearrange_arguments:
    cld
    push rdi
    push rsi
    push rsi
    push rcx
    push rdx
    mov rdx, r8
    mov rcx, r9
    mov rax, rdi ;Function pointer
	mov r9, rsi ;Number of arguments
    pop rsi
    pop rdi
	cmp r9, 0x5
	jb .2
	je .1
	sub r9, 0x6
	je .4
.3:
	push qword [rbp + r9 * 8 + 24]
	dec r9
	jne .3
.4:
	mov r9, [rbp+24]
.1:
	mov r8, [rbp+16]
.2:
	call rax
	pop r9 ; Number of arguments
	sub r9, 0x6
	jbe .6
.5:
	pop r8
	dec r9
	jne .5
.6:
	pop rsi
	pop rdi
	;mov %[output_value], rax ;rax already contains the return value.
	cld
	ret

section .data

align 64
x87_state: times 0x200 db 0x0