
#ifndef _ATA_H
#define _ATA_H

#include <stdint.h>
#include <stddef.h>

#define ATA_SECTOR_SIZE 512

// IO registers
#define ATA_DATA_REG        0x01F0
#define ATA_ERR_FEAT_REG    0x01F1
#define ATA_SCTR_COUNT_REG  0x01F2
#define ATA_LBA_LOW_REG     0x01F3
#define ATA_LBA_MID_REG     0x01F4
#define ATA_LBA_HIGH_REG    0x01F5
#define ATA_DRV_HD_REG      0x01F6
#define ATA_STAT_CMD_REG   0x01F7

// drives
#define ATA_DRIVE_MASTER    0xA0
#define ATA_DRIVE_SLAVE     0xB0

// error codes
#define ATA_ERR_AMNF  0x1
#define ATA_ERR_TKZNF 0x2
#define ATA_ERR_ABRT  0x4
#define ATA_ERR_MCR   0x8
#define ATA_ERR_IDNF  0x10
#define ATA_ERR_MC    0x20
#define ATA_ERR_UNC   0x40
#define ATA_ERR_BBK   0x80

// Status codes
#define ATA_STATUS_ERR  0x01  // indicated an error occured
#define ATA_STATUS_IDX  0x02  // always set to 0
#define ATA_STATUS_CORR 0x04  // always set to 0
#define ATA_STATUS_DRQ  0x08  // set when the drive has PIO data to transfer, or is ready to accept PIO data
#define ATA_STATUS_SRV  0x10  // overlapped mode service request
#define ATA_STATUS_DF   0x20  // 0b00100000
#define ATA_STATUS_RDY  0x40  // only cleared when drive is spun down, or after error
#define ATA_STATUS_BSY  0x80  // set when the drive is preping to send/receive data

// commands
#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_PACKET 0xA0

#define ATA_ID_RET_LBA48        0x53
#define ATA_ID_RET_LBA48_BIT    0x400
#define ATA_ID_RET_LBA28_BLOCK  0x3C
#define ATA_ID_RET_LBA48_BLOCK  0x64



// 400ns delay when switching drives
#define ATA_SELECT_DELAY(bus) \
  {inportb(bus+0x206);inportb(bus+0x206);inportb(bus+0x206);inportb(bus+0x206);}

extern uint64_t ATA_SECTOR_COUNT(void);

extern void ATA_identify(int drive);
extern void ATA_read_sectors(void *target_address, uint32_t LBA, uint32_t sector_count);
extern void ATA_write_sectors(void *target_address, uint32_t LBA, uint32_t sector_count);

extern void ATA_wipe_disk(void);
extern void ATA_init(void);
//extern uint64_t ATA_drive_size(int drive_num, int *block_size);


#endif
