#include "kernel/pci.h"
#include "kernel/term/tty.h"
#include <stdint.h>

void cos_lspci_command(int32_t argc, char argv[32][20])
{
    (void)argc;
    (void)argv;
    for (uint16_t bus = 0; bus < 256; ++bus) {
        for (uint8_t device = 0; device < 32; ++device) {
            for (uint8_t function = 0; function < 8; ++function) {
                uint16_t vendor = pci_config_read16(bus, device, function, 0x00);
                if (vendor == 0xFFFF)
                    continue; // No device
                cos_printf("Found PCI Device at %d:%b.0%d\n", bus, device, function);
            }
        }
    }
}
