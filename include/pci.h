#ifndef _PCI_H
#define _PCI_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define   PCI_ADDR            0x0CF8
#define   PCI_DATA            0x0CFC

#define   PCI_MAX_BUS         64        // PCI specs say 256 (we go to 64)
#define   PCI_MAX_DEV         32        // limit
#define   PCI_MAX_FUNC        8         // limit

// PCI vendor codes
#define   PCI_VENDOR_INVALID  0xFFFF
#define   PCI_VENDOR_INTEL    0x8086    // classic

// ATA class and its subclasses
#define   PCI_INTEL_CLS_ATA   0x01

#define   PCI_ATA_SUB_SCSI    0x00
#define   PCI_ATA_SUB_IDE     0x01
#define   PCI_ATA_SUB_FDC     0x02
#define   PCI_ATA_SUB_IPI     0x03
#define   PCI_ATA_SUB_RAID    0x04
#define   PCI_ATA_SUB_ATA     0x05
  #define   PCI_ATA_PRCL_SNGL_DMA   0x20
  #define   PCI_ATA_PRCL_CHND_DMA   0x30
#define   PCI_ATA_SUB_SATA    0x06
  #define   PCI_SATA_PRCL_VNDR_SPC  0x00
  #define   PCI_SATA_PRCL_AHCI      0x01
#define   PCI_ATA_SUB_SAS     0x07
#define   PCI_ATA_SUB_SSS     0x08
  #define   PCI_SSS_PRCL_CTRLR      0x01
  #define   PCI_SSS_PRCL_NVMHCI     0x01
  #define   PCI_SSS_PRCL_NVMHCI_EN  0x01
#define   PCI_ATA_SUB_OTHER   0x80


typedef struct PCI_POS {
  uint8_t  bus;
  uint8_t  dev;
  uint8_t  func;
} PCI_POS;

typedef struct PCI_DEV {
  uint16_t vendor_id;             // Vendor ID = FFFFh if not 'a valid entry'
  uint16_t device_id;
  uint16_t command_reg;
  uint16_t status_reg;
  uint8_t  revision_id;
  uint8_t  program_interface;
  uint8_t  sub_class;
  uint8_t  class_code;
  uint8_t  cache_line_size;
  uint8_t  latency_timer;
  uint8_t  header_type;           // Header Type (0,1,2, if bit 7 set, multifunction device)
  uint8_t  selftest_res;
  uint32_t io_base_addr0;
  uint32_t io_base_addr1;
  uint32_t io_base_addr2;
  uint32_t io_base_addr3;
  uint32_t io_base_addr4;
  uint32_t io_base_addr5;
  uint32_t cardbus_cis_pntr;
  uint16_t subsys_vendor_id;
  uint16_t subsys_id;
  uint32_t exp_rom_base;
  uint8_t  capsoff;
  uint8_t  reserved0[3];
  uint32_t reserved1;
  uint8_t  interrupt_line;        // Interrupt line (0 = none, 1 = IRQ1, etc.)
  uint8_t  interrupt_pin;         // Interrupt pin (0 = none, 1 = INTA, etc.)
  uint8_t  min_time;              // that bus master needs pci bus ownership
  uint8_t  max_time;              // max latency
  uint32_t varies[48];            // varies by device.
} PCI_DEV;


extern void pci_detect_dev_cntlr(void);





#endif
