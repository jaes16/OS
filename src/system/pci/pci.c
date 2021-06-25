#include <pci.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <libc/string.h>
#include <libc/stdio.h>


unsigned char pci_inportb(const int bus, const int dev, const int func, const int port)
{
  const int shift = ((port & 3) * 8);
  const uint32_t val = 0x80000000 |
    ((bus & 0xff) << 16) | // bus
    ((dev & 0x3f) << 11) |
    ((func & 7) << 8) |
    (port & 0xfc);
  outportd(PCI_ADDR, val);
  return (inportd(PCI_DATA) >> shift) & 0xff;
}

unsigned short pci_inportw(const int bus, const int dev, const int func, const int port)
{
  if ((port & 3) <= 2) {
    const int shift = ((port & 3) * 8);
    const uint32_t val = 0x80000000 |
      ((bus & 0xff) << 16) | // bus
      ((dev & 0x3f) << 11) |
      ((func & 7) << 8) |
      (port & 0xfc);
    outportd(PCI_ADDR, val);
    return (inportd(PCI_DATA) >> shift) & 0xffff;
  } else {
    return (pci_inportb(bus, dev, func, port + 1) << 8) | pci_inportb(bus, dev, func, port);
  }
}

unsigned int pci_inportd(const int bus, const int dev, const int func, const int port)
{
  if ((port & 3) == 0) {
    const uint32_t val = 0x80000000 |
      ((bus & 0xff) << 16) | // bus
      ((dev & 0x3f) << 11) |
      ((func & 7) << 8) |
      (port & 0xfc);
    outportd(PCI_ADDR, val);
    return inportd(PCI_DATA);
  } else {
    return (pci_inportw(bus, dev, func, port + 2) << 16) | pci_inportw(bus, dev, func, port);
  }
}


void pci_detect_dev_cntlr(void)
{

  for (int pci_bus = 0; pci_bus < PCI_MAX_BUS; pci_bus++) {
    for (int pci_dev = 0; pci_dev < PCI_MAX_DEV; pci_dev++) {
      for (int pci_func = 0; pci_func < PCI_MAX_FUNC; pci_func++) {

        if (pci_inportw(pci_bus, pci_dev, pci_func, 0) != PCI_VENDOR_INVALID) {

          PCI_DEV pci_config_sp;
          for (int i=0; i < 64; i++) ((int *) &pci_config_sp)[i] = pci_inportd(pci_bus, pci_dev, pci_func, (i<<2));

          printf("Found device:\n");
          printf("\tVendor %x, Device %x, rev = %x\n"
                   "\tClass = %x, Subclass = %x, Protocol = %x\n"
                   "\tBus = %d, Dev = %d, Func = %d\n",
                   pci_config_sp.vendor_id, pci_config_sp.device_id, pci_config_sp.revision_id,
                   pci_config_sp.class_code, pci_config_sp.sub_class, pci_config_sp.program_interface,
                   pci_bus, pci_dev, pci_func);


          if (pci_config_sp.vendor_id == PCI_VENDOR_INTEL){
            if (pci_config_sp.class_code == PCI_INTEL_CLS_ATA) { // ATA boy

              printf("\tDevice is from Intel and ATA compatible...\n");
              printf("\tDevice base address (from 0x08000000):\n"
                      "\t\t0: %x, 1: %x, 2: %x,\n"
                      "\t\t3: %x, 4: %x, 5: %x\n",
                      pci_config_sp.io_base_addr0, pci_config_sp.io_base_addr1, pci_config_sp.io_base_addr2,
                    pci_config_sp.io_base_addr3, pci_config_sp.io_base_addr4, pci_config_sp.io_base_addr5);


              // check subclass of ATA
              switch (pci_config_sp.sub_class){
                case PCI_ATA_SUB_SCSI:
                  break;
                case PCI_ATA_SUB_IDE:
                  printf("\tDevice is an IDE ATA device:\n");
                  printf("\t\tPrimary: %s, Secondary: %s", (pci_config_sp.program_interface & (1 << 0)) ? "native" : "compatibility (0x01F0)",
                    (pci_config_sp.program_interface & (1 << 2)) ? "native" : "compatibility (0x0170)");
                  break;
                case PCI_ATA_SUB_FDC:
                  break;
                case PCI_ATA_SUB_IPI:
                  break;
                case PCI_ATA_SUB_RAID:
                  break;
                case PCI_ATA_SUB_ATA:
                  break;
                case PCI_ATA_SUB_SATA:
                  break;
                case PCI_ATA_SUB_SAS:
                  break;
                case PCI_ATA_SUB_SSS:
                  break;
                case PCI_ATA_SUB_OTHER:
                  break;
            }

            // check if port io, or memory address io
            if ((pci_config_sp.io_base_addr0 & 1) == 0){
              // memory address io
              // address is 16 byte aligned:
              uint64_t dev_mem_base_addr = ((uint64_t) pci_config_sp.io_base_addr0 & 0xfffffff0);


              uint8_t dev_addr_spc = ((uint8_t) pci_config_sp.io_base_addr0 & 0xf);
              // if bits 2 and 1 == 00b, address space is anywhere in 4gb space
              // if bits 2 and 1 == 01b, address space is below 1mb space
              // if bits 2 and 1 == 10b, address space is anywhere in 64bit address space, and the high 32 bits are in the next addr
              if ((dev_addr_spc & 2) != 0) dev_mem_base_addr |= (uint64_t) pci_config_sp.io_base_addr1 << 32;
            }
            else { // port io

            }

            }
          }
        }

      }

    }

  }
}
