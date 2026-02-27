bits 32
global start
global kernel_stack_top
global multiboot_pointer
global page_table_level_4
global VIRT_ADDR
extern long_mode_start

section .text

; The 64bit kernel will run as a "Higher Half" kernel.
; It means that, while it is physically loaded into lower addresses, the CPU will treat it as running in high addresses via virtual addressing.
; As it was linked with the 64bit kernel base virtual address, it must be substracted from references to immediate addresses.
; CALL instructions are assembled with relative addressing.
VIRT_ADDR equ 0xFFFFFF8000000000 ; Kernel base virtual address.


start:
	cli													; Interrupts are disabled before initialisation of the kernel environment.
	mov esp, kernel_stack_top - VIRT_ADDR						; Moves the stack pointer on top of the stack.	
	call check_for_multiboot_header
	mov edi, ebx										; After initialisation by GRUB, a pointer to information about the kernel (called the multiboot structure) is stored in ebx.
														; While EBX isn't used as part of the initialisation, it is moved into EDI for safekeeping.
	call check_for_cpuid_support
	call check_for_long_mode
	call enable_paging
	lgdt [gdt32.pointer - VIRT_ADDR]
	jmp gdt32.code_segment:long_mode_start - VIRT_ADDR	; A long jump to a 64bit section is required to update the code segment.

	; This section can never occur. If it does, something terribly wrong has happened.
	jmp error

check_for_multiboot_header:
	cmp eax, 0x36d76289 		; Before the kernel is loaded, this checks whether or not the kernel was correctly loaded in a multiboot2 complient way.
	;cmp eax, 0x2BADB002			; Before the kernel is loaded, this checks whether or not the kernel was correctly loaded in a multiboot1 complient way.

; TODO: Remove failure to load and instead load the kernel in protected mode 32bits.
	jne error					; Before start is called, GRUB move the value 0x36D76289 into eax.
	ret

; TODO: Add support for other ways to load long mode without CPUID as some processors may support IA-32e without having the CPUID instruction.
check_for_cpuid_support:; The CPUID instruction posesses a mechanism for easy switch from IA-32 into IA-32e (64bit/long mode).
						; We check whether or not this CPU supports it.
						; TODO: Boot the kernel into IA-32 in protected mode instead if it doesn't.
	pushfd				; We start by storing the Eflags in sthe stack.
	pop eax				; The first value is poped from the stack into eax, since the ID bit is the last of the eflags register, it means its the value at the top of the stack.
	and eax, 1 << 21
	jnz cpuid_already_exists
	mov ecx, eax		; The value of eax is temporarily backed up into ecx in order to check whether or not turning on the cpuid works.
	or eax, 1 << 21   	; To enable support for CPUID, the ID bit of the eflags registers must be switched to 1.
	push eax			; Restores the stack by pushing the missing value of the eflags registers.
	popfd				; This instruction will try to switch the ID bit on by restoring the values from the stack.
	pushfd
	pop eax
	push ecx			; Pushes the backed up value to the stack to leave this function with the cpu state unchanged.
	popfd
	cmp eax, ecx		; If eax and ecx are equal, it means that switching the ID bit doesn't work and most likely means the CPU doesn't have the CPUID instruction.

; TODO: Remove failure to load and instead load the kernel in protected mode 32bits.
	je error
cpuid_already_exists:
	push eax
	popfd
	ret

; TODO: Add support for other ways to load long mode without CPUID as some processors may support IA-32e without having the CPUID instruction.
check_for_long_mode:
						; The CPUID instruction can return information about the processor depending on the value passed to it via the eax register.
	mov eax, 0x80000000 ; When value 0x8000000 is passed as argument, CPUID will return in eax the maximum value that CPUID can handle on the current processor.
	cpuid
	cmp eax, 0x80000001 ; If eax is inferior to 0x80000001, it means the CPU most likely cannot enter long mode.
	jb error

	mov eax, 0x80000001 ; This value will return in ecx and edx information about the CPU.
	cpuid
	test edx, 1 << 29	; Bit 29 in edx is flipped to 1 if 64bit support exists in the CPU.

; TODO: Remove failure to load and instead load the kernel in protected mode 32bits.
	jz error
	ret

; Note: Control Registers (crX) can't receive immediate values and can only be edited via other registers.
; 		MSRs can only be interacted with via the rdmsr and wrmsr instructions.
enable_paging:

	cli
	mov eax, cr4							; Control Register 4's 5th bit switches on the Physical Address Extension.
	or eax, 1 << 5							; This allows 32bit processes to be located above the 4GB RAM limit imposed by the 32bit architecture.
											; However, their virtual address space remains bound to a 4GB RAM limit.
	mov cr4, eax							; As it is required to switch to long mode, it is set to 1.

	mov eax, page_table_level_4 - VIRT_ADDR	; The pointer to the level 4 page has to be passed to the CPU through the control register 3.
	mov cr3, eax

	mov ecx, 0xC0000080					; The instruction RDMSR takes its argument from ecx. MSR 0xC0000080 controls long mode features.
	rdmsr								; Once this instruction returns, it stores the msr information in eax.
	or eax, 1 << 8						; Bit 8 enables or disables long mode. It is flipped to 1 to enable it.
	wrmsr								; The instruction WRMSR takes its argument from eax and it changes the value of the MSR specified in ecx.

	mov eax, cr0
	or eax, 0x80000001					; Bit 31 and 1 of control register 0 control the paging and protected mode features respectively.
	mov cr0, eax						; They're switched to 1 to enable paging (and enable protected mode if it wasn't already [GRUB launches this kernel in 32bit protected mode by default though.])

	ret

error:									; Prints the Error.
	mov dword [0xb8000], 0x4f524f45
	mov dword [0xb8004], 0x4f3a4f52
	mov dword [0xb8008], 0x4f204f20

; The CPU will remain in this state indefinitely until it is rebooted.
loop_halt_cpu:
	hlt
	jmp loop_halt_cpu

section .data

; The page tables have 4 levels of indirection represented by the following 4 arrays and each level was given a pointer to the one before it.
; The first entry of PML4 is the identity map of addresses 0x0 - 0x200000 to virtual addresses 0x0 - 0x200000.
; This is temporarily done because the linker script physically loads the kernel at address 1MB from the beginning of RAM and the GRUB multiboot info structure is also located within the first MB of memory as well.
; Due to the way paging works, we set up the last entry of the General Page Directory as the higher half virtual address is generated as being its last entry.

page_table_level_4:
	dq page_table_level_3 - VIRT_ADDR + 0b111	; This flips to 1 the present and writable bits of the page level.
												; Each level has 512 entries.
  	%rep 509
    	dq 0x0									; As such we define the next entries to 0.
  	%endrep
	dq page_table_level_4 - VIRT_ADDR + 0b111	; Entry of the recursive mapping for the page table.
	dq page_table_level_3 - VIRT_ADDR + 0b111	; Entry of the higher half addresses.

page_table_level_3:								; Same thing here but the number of entries taken is still one.
	dq page_table_level_2 - VIRT_ADDR + 0b111
  	%rep 511
    	dq 0x0
  	%endrep

page_table_level_2:								; Same thing here but we need about 64MB of memory so we add pointers to 32 entries containing 512 pages each.
	%assign page_displace 0
	%rep 32
		dq page_table_level_1 - VIRT_ADDR + 0b111 + page_displace
		%assign page_displace (page_displace + 0x1000)
	%endrep
  	%rep 480
    	dq 0x0
  	%endrep

page_table_level_1:								; The entries' addresses are moved 12bits to the left because a page table entry's first 12 bits are used for configurations.
												; As for the physical address itself, it is an identifier for 4096 bytes blocs, meaning entry 0 to 511 will be defined.
  %assign page 0
  %rep 16384
    dq (page * 0x1000) + 0b111
    %assign page (page + 1)
  %endrep

section .bss
align 16
kernel_stack_bottom:
	resb 65536									; The resb command declares unitialised space for operations.
kernel_stack_top:

section .rodata

ACCESSED equ 1 << 40    ;The CPU switches this bit to 1 when the specified segment is accessed. If the segment is stored in Read Only Data and the segment is accessed, it leads to failure.
RW		 equ 1 << 41    ;Sets the Read Write bit. In code segments, it enables read. In data segments, it enables write.
EXEC	 equ 1 << 43    ;Executable Bit. It defines the segment as executable.
DESCTYPE equ 1 << 44    ;Descriptor type bit. When set to 0, it is a system segment. When set to 1, it is a data or code segment.
DPLLOW	 equ 1 << 45    ;Defines the Ring of the segment. Ring 0 is kernel, Ring 3 is userland.
DPLHIGH	 equ 1 << 46    ;Ring 1 and 2 are never used by a modern OS.
PRESENT  equ 1 << 47    ;Sets if a segment is valid or not. Must be switched on for all segments.
LONGBIT	 equ 1 << 53    ;Enables long mode for a segment.

gdt32:
	dq 0
.code_segment: equ $ - gdt32
	dq ACCESSED | EXEC | DESCTYPE | PRESENT | RW | LONGBIT
.dat_segment: equ $ - gdt32
	dq ACCESSED | RW | DESCTYPE | PRESENT
.pointer:
	dw $ - gdt32 - 1
	dq gdt32
