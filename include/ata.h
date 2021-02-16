
#ifndef _ATA_H
#define _ATA_H

#include <stdint.h>
#include <stddef.h>

extern void ATA_PIO_read_sectors(void *target_address, uint32_t LBA, uint8_t sector_count);
extern void ATA_PIO_write_sectors(void *target_address, uint32_t LBA, uint8_t sector_count);


#endif
