OUTPUT_FORMAT(elf64-x86-64)

ENTRY(start)

SECTIONS
{
	virt_addr = 0xFFFFFF8000000000; /* the code should be relocated to 4GB + 1MB*/
	
	. = 0x100000;
	.multiboot BLOCK(4K) : ALIGN(4K)
	{
		. = ALIGN(8);
		KEEP(*(.multiboot))
	}

	. += virt_addr;
	.text ALIGN (4K) : AT(ADDR(.text) - virt_addr)
	{
		*(.text)
	}

	.rodata ALIGN (4K) : AT(ADDR(.rodata) - virt_addr)
	{
		*(.rodata*)
	}

	.data ALIGN (4K) : AT(ADDR(.data) - virt_addr)
	{
		*(.data)
	}

	.bss ALIGN (4K) : AT(ADDR(.bss) - virt_addr)
	{
		*(COMMON)
		*(.bss)
	}

	kernel_end_address = . - virt_addr;
}
