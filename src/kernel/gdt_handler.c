#include "kernel/gdt.h"
#include "kernel/cos_memory.h"

tss_t tss = {0};
cos_gdt_entry_t gdt64[7] = {0};
cos_gdt_ptr_t gdt64_ptr = {0};
extern void reload_segments(void);

static void setup_gdt_entry(uint8_t nb_entry, uint32_t base, uint32_t limit, uint16_t flags)
{
    cos_gdt_entry_t entry = {0};

    entry.base0 = base & 0xFFFF;
    entry.base1 = (base >> 16) & 0xFF;
    entry.base2 = (base >> 24) & 0xFF;
    entry.limit0 = limit & 0xFFFF;
    entry.limit1 = (limit >> 16) & 0xF;
    entry.flags0 = flags & 0xFF;
    entry.flags1 = (flags >> 8) & 0xF;

    gdt64[nb_entry] = entry;
}

void reload_tss(uint64_t stack_ptr)
{
    cos_gdt_entry_tss_high_t tss_high = {0};

    tss.rsp0 = (uint64_t)stack_ptr;
    tss.iopb = sizeof(tss_t) - 1;
    tss_high.base3 = ((uint64_t)&tss >> 32) & 0xFFFFFFFF;

    setup_gdt_entry(5, ((uint64_t)&tss) & 0xFFFFFFFF, sizeof(tss_t) - 1, 0x89);//PRESENT | ACCESSED | EXEC);
    cos_memcpy(&gdt64[6], &tss_high, sizeof(tss_high));
    __asm__ __volatile__ (
        "lgdt %[gdt_ptr]\n"
        "ltr %%ax\n"
        :
        : [gdt_ptr] "m" (gdt64_ptr), "a" (0x28)
    );
    reload_segments();
}

void setup_gdt(void)
{
    extern unsigned long tss_stack_top[];

    gdt64_ptr.gdt_size = sizeof(gdt64) - 1;
    gdt64_ptr.gdt_ptr = (uint64_t)&gdt64;

    setup_gdt_entry(0, 0, 0x0, 0x0);
    setup_gdt_entry(1, 0, 0xFFFFF, ACCESSED | EXEC | DESCTYPE | PRESENT | RW | LONGBIT);
    setup_gdt_entry(2, 0, 0xFFFFF, ACCESSED | RW | DESCTYPE | PRESENT);
    setup_gdt_entry(4, 0, 0xFFFFF, ACCESSED | EXEC | DESCTYPE | DPLLOW | DPLHIGH | PRESENT | RW | LONGBIT);
    setup_gdt_entry(3, 0, 0xFFFFF, ACCESSED | RW | DESCTYPE | DPLLOW | DPLHIGH | PRESENT);
    reload_tss((uint64_t)tss_stack_top);
}

extern void cos_syscall80(void);

void enable_syscall(void)
{
    uint64_t addr = (uint64_t)cos_syscall80;

    __asm__ __volatile__ (
        "mov $0xC0000082, %%rcx\n"
        "mov %0, %%rax\n"
        "mov %%rax, %%rdx\n"
        "shr $32, %%rdx\n"
        "wrmsr\n"

        "mov $0xC0000080, %%rcx\n"
        "rdmsr\n"
        "or $0x1, %%rax\n"
        "wrmsr\n"

        "mov $0xC0000081, %%rcx\n"
        "xor %%rax, %%rax\n"
        "mov $0x00130008, %%edx\n"
        "wrmsr\n"

        :: "r" (addr)
        : "rax", "rcx", "rdx"
    );
}
