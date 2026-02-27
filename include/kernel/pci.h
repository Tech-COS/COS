#pragma once

#include <stdint.h>
#include <stdbool.h>

bool find_and_switch_ide_controller();
uint8_t pci_config_read8(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint16_t pci_config_read16(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
