#include <stdbool.h>
#include <stdint.h>
#include "kernel/cos_memory.h"
#include "types.h"
#include "kernel/lib/string.h"
#include "kernel/term/tty.h"
#include "kernel/acpi.h"

bool calculate_acpi_checksum(void *acpi_struct, uint64_t size,  const char *str)
{
    uint64_t rsdp_table_checksum_check = 0;

    for (uint64_t i = 0; i < size; ++i)
        rsdp_table_checksum_check += ((uint8_t *)acpi_struct)[i];
    if (rsdp_table_checksum_check & 0xFF) {
        cos_printf("FATAL: %s TABLE CANNOT BE TRUSTED\n", str);
        return false;
    }
    return true;
}

uint32_t fetch_xsdt_table_phys(struct XSDP_t *xsdp)
{
    uint32_t rsdt_addr_phys = 0;

    if (!xsdp)
        return 0;
    if (cos_strncmp("RSD PTR ", xsdp->Signature, 8))
        cos_printf("XSDP Table is probably invalid.\n");
    if (!calculate_acpi_checksum(xsdp, sizeof(struct RSDP_t),  "XSDP") || !calculate_acpi_checksum(xsdp, sizeof(struct XSDP_t), "XSDP"))
        return 0;
    cos_printf("XSDP Signature: %c%c%c%c%c%c%c%c\n", xsdp->Signature[0], xsdp->Signature[1], xsdp->Signature[2], xsdp->Signature[3], xsdp->Signature[4], xsdp->Signature[5], xsdp->Signature[6], xsdp->Signature[7]);
    cos_printf("XSDP OEM: %c%c%c%c%c%c\n", xsdp->OEMID[0], xsdp->OEMID[1], xsdp->OEMID[2], xsdp->OEMID[3], xsdp->OEMID[4], xsdp->OEMID[5]);
    cos_printf("XSDP Revision is %X\n", xsdp->Revision);
    cos_printf("XSDT_t Address is %X\n", xsdp->RsdtAddress);
    cos_printf("XSDP Checksum is %X\n", xsdp->Checksum);
    cos_printf("XSDP Length is %X\n", xsdp->Length);
    cos_printf("XSDT_t Address is %X\n", xsdp->XsdtAddress);
    cos_printf("XSDP ExtendedChecksum is %X\n", xsdp->ExtendedChecksum);
    if (!xsdp->Revision) {
        cos_printf("ACPI Version 1.0 is used\n");
        rsdt_addr_phys = xsdp->RsdtAddress;
    } else {
        cos_printf("ACPI Version 2.0 or more is used\n");
        rsdt_addr_phys = (uint32_t)xsdp->XsdtAddress;
    }
    return rsdt_addr_phys;
}

uint32_t fetch_rsdt_table_phys(struct RSDP_t *rsdp)
{
    if (!rsdp)
        return 0;
    if (cos_strncmp("RSD PTR ", rsdp->Signature, 8))
        cos_printf("RSDP Table is probably invalid.\n");
    if (!calculate_acpi_checksum(rsdp, sizeof(struct RSDP_t), "RSDP"))
        return 0;
    cos_printf("RSDP Signature: %c%c%c%c%c%c%c%c\n", rsdp->Signature[0], rsdp->Signature[1], rsdp->Signature[2], rsdp->Signature[3], rsdp->Signature[4], rsdp->Signature[5], rsdp->Signature[6], rsdp->Signature[7]);
    cos_printf("RSDP OEM: %c%c%c%c%c%c\n", rsdp->OEMID[0], rsdp->OEMID[1], rsdp->OEMID[2], rsdp->OEMID[3], rsdp->OEMID[4], rsdp->OEMID[5]);
    cos_printf("RSDP Revision is %X\n", rsdp->Revision);
    cos_printf("RSDT_t Address is %X\n", rsdp->RsdtAddress);
    cos_printf("RSDP Checksum is %X\n", rsdp->Checksum);
    cos_printf("ACPI Version 1.0 is used\n");
    return rsdp->RsdtAddress;
}

void discover_acpi(struct RSDP_t *rsdp, struct XSDP_t *xsdp)
{
    uint32_t rsdt_addr_phys = 0;
    uint64_t valid_entries = 0;
    struct XSDT_t *full_acpi_xsdt_structure = NULL;
    struct RSDT_t *full_acpi_rsdt_structure = NULL;

    if (!rsdp && !xsdp)
        return;
    if (xsdp) {
        if (!(rsdt_addr_phys = fetch_xsdt_table_phys(xsdp)))
            return;
        cos_printf("XSDT_t PHYSICAL ADDRESS: %X\n", rsdt_addr_phys);
        full_acpi_xsdt_structure = (void *)init_reserved_memory(rsdt_addr_phys, sizeof(struct XSDT_t));
        if (sizeof(struct XSDT_t) < full_acpi_xsdt_structure->header.Length)
            return;
        if (!calculate_acpi_checksum(full_acpi_xsdt_structure, full_acpi_xsdt_structure->header.Length, "XSDT_t"))
            return;
        valid_entries = (full_acpi_xsdt_structure->header.Length - sizeof(struct ACPISDTHeader)) / 8;
        cos_printf("Number of valid XSDT Entries: %d\n", valid_entries);
    } else {
        if (!(rsdt_addr_phys = fetch_rsdt_table_phys(rsdp)))
            return;
        cos_printf("RSDT_t PHYSICAL ADDRESS: %X\n", rsdt_addr_phys);
        full_acpi_rsdt_structure = (void *)init_reserved_memory(rsdt_addr_phys, sizeof(struct RSDT_t));
        if (sizeof(struct RSDT_t) < full_acpi_rsdt_structure->header.Length)
            return;
        if (!calculate_acpi_checksum(full_acpi_rsdt_structure, full_acpi_rsdt_structure->header.Length, "RSDT_t"))
            return;
        valid_entries = (full_acpi_rsdt_structure->header.Length - sizeof(struct ACPISDTHeader)) / 4;
        cos_printf("Number of valid RSDT Entries: %d\n", valid_entries);
    }
}
