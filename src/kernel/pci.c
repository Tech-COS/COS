#include "kernel/asm.h"
#include "kernel/term/tty.h"
#include <stdint.h>
#include <stdbool.h>


#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC
#define PCI_CLASS_MASS_STORAGE 0x01
#define PCI_SUBCLASS_IDE 0x01
#define PCI_PROGIF_NATIVE_MODE_WITH_LEGACY_SUPPORT 0x8F
#define VENDOR_INTEL 0x8086
#define VENDOR_INVALID 0xFFFF

#define PCI_ADDR(bus, device, func, offset) (1U << 31) | ((uint32_t)bus << 16) | ((uint32_t)device << 11) | ((uint32_t)func << 8) | (offset & 0xFC)

uint32_t fetch_pci_config_data(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    uint32_t address = PCI_ADDR(bus, device, func, offset);
    cos_out32(CONFIG_ADDRESS, address);
    return cos_in32(CONFIG_DATA);
}

uint16_t pci_config_read16(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    return (uint16_t)((fetch_pci_config_data(bus, device, func, offset) >> ((offset & 2) * 8)) & 0xFFFF);
}

uint8_t pci_config_read8(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
    return (uint8_t)((fetch_pci_config_data(bus, device, func, offset) >> ((offset & 3) * 8)) & 0xFF);
}

void pci_config_write8(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint8_t value)
{
    uint32_t pci_config_data = fetch_pci_config_data(bus, device, func, offset);
    uint8_t shift = (offset & 3) * 8;

    pci_config_data &= ~(0xFF << shift);
    pci_config_data |= ((uint32_t)value << shift);
    cos_out32(CONFIG_ADDRESS, PCI_ADDR(bus, device, func, offset));
    cos_out32(CONFIG_DATA, pci_config_data);
}

void pci_config_write16(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint16_t value)
{
    uint32_t pci_config_data = fetch_pci_config_data(bus, device, func, offset);
    uint8_t shift = (offset & 2) * 8;

    pci_config_data &= ~(0xFFFF << shift);
    pci_config_data |= ((uint32_t)value << shift);
    cos_out32(CONFIG_ADDRESS, PCI_ADDR(bus, device, func, offset));
    cos_out32(CONFIG_DATA, pci_config_data);
}

bool find_and_switch_ide_controller(void)
{
    uint16_t vendor = 0;
    uint16_t device_id = 0;
    uint8_t class_code = 0;
    uint8_t subclass = 0;
    uint8_t prog_if = 0;

    for (uint16_t bus = 0; bus < 256; ++bus) {
        for (uint8_t device = 0; device < 32; ++device) {
            for (uint8_t function = 0; function < 8; ++function) {

                vendor = pci_config_read16(bus, device, function, 0x00);
                if (vendor == VENDOR_INVALID)
                    continue;

                device_id = pci_config_read16(bus, device, function, 0x02);
                class_code = pci_config_read8(bus, device, function, 0x0B);
                subclass   = pci_config_read8(bus, device, function, 0x0A);
                prog_if    = pci_config_read8(bus, device, function, 0x09);

                if (vendor == VENDOR_INTEL && class_code == PCI_CLASS_MASS_STORAGE && subclass == PCI_SUBCLASS_IDE) {

                    cos_printf("Found Intel IDE Controller at %x:%x.%d\n", bus, device, function);
                    cos_printf("Vendor ID: 0x%X\n", vendor);
                    cos_printf("Device ID: 0x%X\n", device_id);

                    if (!(prog_if & 0b101)) {
                        cos_printf("Both channels of the controller are already in Legacy Mode.\n");
                        return true;
                    }
                    if ((prog_if & 0b1010)) {

                        cos_printf("The disk controller reported that it can be switched to Legacy Mode.\n");
                        cos_printf("Trying to switch the controller to Legacy Mode...\n");

                        pci_config_write8(bus, device, function, 0x09, prog_if & ~((1 << 0) | (1 << 2)));

                        prog_if = pci_config_read8(bus, device, function, 0x09);
                        if ((prog_if & ((1 << 0) | (1 << 2))) == 0) {
                            cos_printf("The drive controller was successfully switched to Legacy Mode.\n");
                            return true;
                        } else {
                            cos_printf("The drive controller state hasn't changed and failed to switch to Legacy Mode.\n");
                            cos_printf("You may turn off your computer as your system is in an unsupported state.\n");
                            cos_printf("To do so, press the Power Button until the screen turns off.");
                            return false;
                        }
                    }
                }
            }
        }
    }
    cos_printf("No Intel IDE controller with Legacy Mode could be found.\n");
    cos_printf("You may turn off your computer as your system is in an unsupported state.\n");
    cos_printf("To do so, press the Power Button until the screen turns off.");
    return false;
}

