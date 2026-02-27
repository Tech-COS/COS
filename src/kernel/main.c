////////////////////////
//
//  Created: Thu Jun 27 2024
//  File: main.c
//
////////////////////////

#include "kernel/graphics/bmp.h"
#include "kernel/pci.h"
#include "kernel/term/tty.h"
#include "kernel/idt.h"
#include "kernel/term/term.h"
#include "kernel/multiboot2.h"
#include "kernel/cos_memory.h"
#include "kernel/cos_filesystem.h"
#include <stdint.h>
#include "kernel/graphics/font.h"
#include "kernel/graphics/screen.h"
#include "kernel/acpi.h"
#include "../FileSystem/include/env.h"
#include "../Scheduler/include/scheduling.h"
#include "kernel/lib/string.h"

extern char _binary_assets_COS_Logo_bmp_start;
extern char _binary_assets_COS_Logo_bmp_end;

void cos_kernel_main(unsigned long mbd)
{
    extern unsigned long kernel_end_address[];
    uint64_t framebuffer_addr = 0;
    uint64_t page_frame_nb = 0;
    struct multiboot_tag_framebuffer *frametag = NULL;
    struct multiboot_tag_mmap *memtag = NULL;
    struct multiboot_tag *tag = NULL;

    struct RSDP_t *rsdp = NULL;
    struct XSDP_t *xsdp = NULL;

    char *input_buffer = NULL;
    uint8_t disk_params = 0;
    uint64_t max_sectors = 0;


    uint64_t privileged_addr[256];
    uint64_t i = 0;


    cos_term_init();
    cos_idt_init();
    if (mbd & 7) {
        cos_printf("0x%x UNALIGNED VALUES AHEAD!", mbd);
        return;
    }

    for (tag = (struct multiboot_tag *)(mbd + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MMAP:
                memtag = (struct multiboot_tag_mmap *)tag;
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                frametag = (struct multiboot_tag_framebuffer *)tag;
                privileged_addr[i++] = frametag->common.framebuffer_addr;
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                rsdp = (struct RSDP_t *)((struct multiboot_tag_old_acpi *)tag)->rsdp;
                privileged_addr[i++] = rsdp->RsdtAddress;
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                xsdp = (struct XSDP_t *)((struct multiboot_tag_new_acpi *)tag)->rsdp;
                privileged_addr[i++] = xsdp->XsdtAddress;
                break;
        }
    }


    page_frame_nb = cos_mmap_init(mbd, memtag, (unsigned long)kernel_end_address, privileged_addr);
    init_scheduler();

    framebuffer_addr = init_reserved_memory(frametag->common.framebuffer_addr, frametag->common.framebuffer_height * frametag->common.framebuffer_width * (frametag->common.framebuffer_bpp >> 3));
    memset((void *)framebuffer_addr, 0, frametag->common.framebuffer_height * frametag->common.framebuffer_width * (frametag->common.framebuffer_bpp >> 3));
    psf_init();
    init_screen(frametag, (void *)framebuffer_addr);
    rgb_screen_t *screen = get_screen();
    cos_term_clear();
    cos_printf("Initial Screen Size: %d - %d | %d | %d\n", screen->frametag->common.framebuffer_width, screen->frametag->common.framebuffer_height, screen->frametag->common.framebuffer_pitch, (frametag->common.framebuffer_bpp >> 3));
    cos_printf("Screen Size: %d - %d\n", get_screen_width(), get_screen_height());
    cos_printf("Number of blocks claimed: %d\n",page_frame_nb);
    if (page_frame_nb == 0) {
        cos_printf("FATAL: ALL AVAILABLE MEMORY WAS CONSUMED");
        while (1);
    }

    //discover_acpi(rsdp, xsdp);

    if (!find_and_switch_ide_controller())
        while(1);

    disk_params = discover_disk(&max_sectors, 0);
    if (disk_params & LBA28_BIT)
        cos_printf("%s\n", "Drive supports LBA28 Mode.");
    if (disk_params & LBA48_BIT)
        cos_printf("%s\n", "Drive supports LBA48 Mode.");
    if (disk_params & UDMA_SUPPORT_BIT)
        cos_printf("%s\n", "Drive supports UDMA > 2.\n");
    cos_printf("%s%d%s\n", "Drive supports ", max_sectors, " LBA sectors.\n");


    if (!detect_ext4_partition_start_from_disk())
        while(1);

    cos_printf("%s", "$/> ");
    while (1) {
        input_buffer = cos_getline();

        if (input_buffer) {
            cos_run_command(input_buffer);
            cos_printf("$%s> ", get_env_path());
        }

    }

    cos_printf("%s\n", "End of Kernel. You mustn't see this message.");
}
