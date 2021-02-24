#include <ata.h>
#include <system.h>
#include <stdint.h>
#include <stddef.h>

#define SECTOR_SIZE 512

// IO registers
#define ATA_PIO_DATA_REG      0x01F0
#define ATA_PIO_ERR_FEAT_REG  0x01F1
#define ATA_PIO_SCTR_COUNT_REG  0x01F2
#define ATA_PIO_LBA_LOW_REG  0x01F3
#define ATA_PIO_LBA_MID_REG   0x01F4
#define ATA_PIO_LBA_HIGH_REG  0x01F5
#define ATA_PIO_DRV_HD_REG    0x01F6
#define ATA_PIO_STAT_CMND_REG 0x01F7

#define ATA_PIO_ERR_AMNF  0x1
#define ATA_PIO_ERR_TKZNF 0x2
#define ATA_PIO_ERR_ABRT  0x4
#define ATA_PIO_ERR_MCR   0x8
#define ATA_PIO_ERR_IDNF  0x10
#define ATA_PIO_ERR_MC    0x20
#define ATA_PIO_ERR_UNC   0x40
#define ATA_PIO_ERR_BBK   0x80

// Status codes
#define ATA_PIO_STATUS_ERR  0x01  // indicated an error occured
#define ATA_PIO_STATUS_IDX  0x2   // always set to 0
#define ATA_PIO_STATUS_CORR 0x4   // always set to 0
#define ATA_PIO_STATUS_DRQ  0x08  // set when the drive has PIO data to transfer, or is ready to accept PIO data
#define ATA_PIO_STATUS_SRV  0x10  // overlapped mode service request
#define ATA_PIO_STATUS_DF   0x20  // 0b00100000
#define ATA_PIO_STATUS_RDY  0x40  // only cleared when drive is spun down, or after error
#define ATA_PIO_STATUS_BSY  0x80  // set when the drive is preping to send/receive data

#define ATA_PIO_CMD_IDENTIFY 0xEC
#define ATA_PIO_CMD_READ 0x20
#define ATA_PIO_CMD_WRITE 0x30
static uint8_t LBA_LOW(uint32_t LBA) { return LBA & 0xff; }
static uint8_t LBA_MID(uint32_t LBA) { return (LBA & 0xff00) >> 8; }
static uint8_t LBA_HIGH(uint32_t LBA) { return (LBA & 0xff0000) >> 16; }
static uint8_t LBA_DRV_HD(uint32_t LBA) { return (0xe0 | ((LBA >> 24) & 0xf) ); }


static void ATA_wait_not_BSY(void)
{ // wait until drive is not busy
	while(inportb(ATA_PIO_STAT_CMND_REG) & ATA_PIO_STATUS_BSY);
}
static void ATA_wait_DRQ(void)
{ // wait until drive is ready to accept PIO data
	while(!(inportb(ATA_PIO_STAT_CMND_REG) & ATA_PIO_STATUS_DRQ));
}
static void ATA_wait_ERR(void)
{ // wait until drive is ready to accept PIO data
	while(inportb(ATA_PIO_STAT_CMND_REG) & ATA_PIO_STATUS_ERR);
}

void ATA_PIO_identify(int drive)
{
  __asm__ __volatile__ ("cli");

  if(drive == 0) outportb(ATA_PIO_DRV_HD_REG, 0xA0); // master drive
  else outportb(ATA_PIO_DRV_HD_REG, 0xB0); // slave drive

  outportb(ATA_PIO_SCTR_COUNT_REG, 0);
  outportb(ATA_PIO_LBA_LOW_REG, 0);
	outportb(ATA_PIO_LBA_MID_REG, 0);
	outportb(ATA_PIO_LBA_HIGH_REG, 0);

  // send out command to identify
  outportb(ATA_PIO_STAT_CMND_REG, ATA_PIO_CMD_IDENTIFY);

  // read in status. if 0, drive does not exist
  uint32_t status = inportb(ATA_PIO_STAT_CMND_REG);

  char status_itoa[32];
  itoa(status, 10, status_itoa);
  //puts("ATA_PIO_identify: status returned from identify command: ");
  //puts(status_itoa);
  //putc('\n');

  if(status == 0){
    puts("ATA_PIO_identify: Drive doesn't exist!\n");
    return;
  }

  // otherwise, wait till drive is not busy, then check LBAmid and LBAhi
  ATA_wait_not_BSY();
  if(inportb(ATA_PIO_LBA_MID_REG) != 0){
    puts("ATA_PIO_identify: LBAmid is not 0! Drive is not ATA!\n");
    return;
  }
  if(inportb(ATA_PIO_LBA_HIGH_REG) != 0){
    puts("ATA_PIO_identify: LBAhigh is not 0! Drive is not ATA!\n");
    return;
  }

  __asm__ __volatile__ ("sti");

  char temp[512];
  ATA_PIO_read_sectors(temp, 0, 1);
}


void ATA_PIO_read_sectors(void *target_address, uint32_t LBA, uint8_t sector_count)
{
  __asm__ __volatile__ ("cli");

	ATA_wait_not_BSY();
	outportb(ATA_PIO_DRV_HD_REG, LBA_DRV_HD(LBA) );
  outportb(ATA_PIO_ERR_FEAT_REG, 0);
	outportb(ATA_PIO_SCTR_COUNT_REG, sector_count);
	outportb(ATA_PIO_LBA_LOW_REG, LBA_LOW(LBA) );
	outportb(ATA_PIO_LBA_MID_REG, LBA_MID(LBA) );
	outportb(ATA_PIO_LBA_HIGH_REG, LBA_HIGH(LBA) );
	outportb(ATA_PIO_STAT_CMND_REG, ATA_PIO_CMD_READ); // send the read command

	uint16_t *target = (uint16_t*) target_address;

	for (uint16_t j = 0; j < sector_count; j++) {
		ATA_wait_not_BSY();

    ATA_wait_DRQ();
    //puts("ATA_PIO_read_sectors: Finished waiting for DRQ\n");

    ATA_wait_ERR();
    //puts("ATA_PIO_read_sectors: Finished waiting for ERR to clear\n");

		for(uint16_t i = 0; i < SECTOR_SIZE/sizeof(uint16_t); i++){
			target[i] = inportw(ATA_PIO_DATA_REG);
      /*
      char target_itoa[16];
      itoa(target[i], 16, target_itoa);
      puts(target_itoa);
      puts(", ");*/
    }
		target += SECTOR_SIZE/sizeof(uint16_t);
	}

  __asm__ __volatile__ ("sti");

}


void ATA_PIO_write_sectors(void *target_address, uint32_t LBA, uint8_t sector_count)
{
  __asm__ __volatile__ ("cli");

	ATA_wait_not_BSY();
	outportb(ATA_PIO_DRV_HD_REG, LBA_DRV_HD(LBA) );
  outportb(ATA_PIO_ERR_FEAT_REG, 0);
	outportb(ATA_PIO_SCTR_COUNT_REG, sector_count);
	outportb(ATA_PIO_LBA_LOW_REG, LBA_LOW(LBA) );
	outportb(ATA_PIO_LBA_MID_REG, LBA_MID(LBA) );
	outportb(ATA_PIO_LBA_HIGH_REG, LBA_HIGH(LBA) );
	outportb(ATA_PIO_STAT_CMND_REG, ATA_PIO_CMD_WRITE); // send the read command

	uint16_t *target = (uint16_t*) target_address;

	for (uint16_t j = 0; j < sector_count; j++) {
		ATA_wait_not_BSY();
		ATA_wait_DRQ();
    //puts("ATA_PIO_write_sectors: Finished waiting for DRQ\n");

    ATA_wait_ERR();
    //puts("ATA_PIO_write_sectors: Finished waiting for ERR to clear\n");

		for(uint16_t i = 0; i < SECTOR_SIZE/sizeof(uint16_t); i++)
			outportw(ATA_PIO_DATA_REG, target[i]);
		target += SECTOR_SIZE/sizeof(uint16_t);
	}

  __asm__ __volatile__ ("sti");

}

void ATA_PIO_init(void)
{
  ATA_PIO_identify(0);
  ATA_PIO_identify(1);
}
