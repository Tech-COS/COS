bits 32
align 8
section .multiboot
; By convention, a multiboot2 GRUB compatible header must be passed to grub.
; The specification asks that all modules are 8 bytes aligned and the header must have 4 byte alignment.
header_start:
    dd 0xe85250d6                                                   ; Multiboot2 magic value is always this value.
    dd 0                                                            ; GRUB will launch 32bit protected mode of the x86 architecture as 0 means protected mode and 4 is 32bit MIPS architecture.
    dd header_end - header_start                                    ; It requires the length of the header.
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; As well as this arbitrary checksum which must be a 32bit sum of 0 when added to the other prior fields.
align 8
bootloader_requests_start:
    dw 1
    dw 0
    dd bootloader_requests_end - bootloader_requests_start
    dd 4
    dd 6
    dd 8
    dd 14
    dd 15
    dd 0
bootloader_requests_end:
align 8
framebuffer_tag_start:
    ; Tag type 5 defines the framebuffer request tag.
    ; Tells GRUB we prefer getting a framebuffer of size 1920x1080 with 32bit depth.
    dw 5
    dw 0
    dd framebuffer_tag_end - framebuffer_tag_start
    dd 1920
    dd 1080
    dd 32
framebuffer_tag_end:

align 8
    dw 0                                                            ; Type 0.
    dw 0                                                            ; Flags are ignored since set to 0.
    dd 8                                                            ; Size of the tags.
header_end:

;align 4
;header_v1_start:
;    dd 0x1BADB002
;    dd 0x3
;    dd 0x100000000 - (0x3 + 0x1BADB002)
;header_v1_end:
