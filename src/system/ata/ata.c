#include <ata.h>

#include <system.h>

#include <stdint.h>
#include <stddef.h>

#include <libc/string.h>
#include <libc/stdio.h>


uint64_t ATA_sectors = 0;

uint64_t ATA_SECTOR_COUNT(void) { return ATA_sectors; }

// masks for reading lba
static uint8_t LBA_LOW(uint32_t LBA) { return LBA & 0xff; }
static uint8_t LBA_MID(uint32_t LBA) { return (LBA & 0xff00) >> 8; }
static uint8_t LBA_HIGH(uint32_t LBA) { return (LBA & 0xff0000) >> 16; }
static uint8_t LBA_DRV_HD(uint32_t LBA) { return (0xe0 | ((LBA >> 24) & 0xf) ); }

static void ATA_wait_not_BSY(void)
{ // wait until drive is not busy
	while(inportb(ATA_STAT_CMD_REG) & ATA_STATUS_BSY);
}
static void ATA_wait_DRQ(void)
{ // wait until drive is ready to accept PIO data
	while(!(inportb(ATA_STAT_CMD_REG) & ATA_STATUS_DRQ));
}
static void ATA_wait_ERR(void)
{ // wait until drive has no error status
	while(inportb(ATA_STAT_CMD_REG) & ATA_STATUS_ERR);
}

void ATA_identify(int drive)
{

  if(drive == 0) outportb(ATA_DRV_HD_REG, ATA_DRIVE_MASTER); // master drive
  else outportb(ATA_DRV_HD_REG, ATA_DRIVE_SLAVE); // slave drive

	// flush out lba and sector
  outportb(ATA_SCTR_COUNT_REG, 0);
  outportb(ATA_LBA_LOW_REG, 0);
	outportb(ATA_LBA_MID_REG, 0);
	outportb(ATA_LBA_HIGH_REG, 0);

  // send out command to identify
  outportb(ATA_STAT_CMD_REG, ATA_CMD_IDENTIFY);

  // read in status. if 0, drive does not exist
  if(inportb(ATA_STAT_CMD_REG) == 0){
    printf("ATA_identify: Drive%d doesn't exist!\n", drive);
    return;
  }
  // otherwise, wait till drive is not busy, then check LBAmid and LBAhi
  ATA_wait_not_BSY();
  if(inportb(ATA_LBA_MID_REG) != 0){
    printf("ATA_identify: LBAmid is not 0! Drive%d is not ATA!\n", drive);
    return;
  }
  if(inportb(ATA_LBA_HIGH_REG) != 0){
    printf("ATA_identify: LBAhigh is not 0! Drive%d is not ATA!\n", drive);
    return;
  }

  ATA_wait_not_BSY();
  ATA_wait_DRQ();
  ATA_wait_ERR();

  uint16_t buf[ATA_SECTOR_SIZE/sizeof(uint16_t)];
  for(uint32_t i = 0; i<ATA_SECTOR_SIZE/sizeof(uint16_t); i++) buf[i] = inportw(ATA_DATA_REG);

	uint64_t drive_sectors = 0;
  if((buf[ATA_ID_RET_LBA48] & ATA_ID_RET_LBA48_BIT) != 0){ // if we are LBA48, get disk size
    ATA_sectors += buf[ATA_ID_RET_LBA48_BLOCK];
		drive_sectors += buf[ATA_ID_RET_LBA48_BLOCK];
    ATA_sectors += ((uint64_t) buf[ATA_ID_RET_LBA48_BLOCK+1]) << 16;
    drive_sectors += ((uint64_t) buf[ATA_ID_RET_LBA48_BLOCK+1]) << 16;
		ATA_sectors += ((uint64_t) buf[ATA_ID_RET_LBA48_BLOCK+2]) << 32;
    drive_sectors += ((uint64_t) buf[ATA_ID_RET_LBA48_BLOCK+2]) << 32;
    ATA_sectors += ((uint64_t) buf[ATA_ID_RET_LBA48_BLOCK+3]) << 48;
    drive_sectors += ((uint64_t) buf[ATA_ID_RET_LBA48_BLOCK+3]) << 48;
  }

	printf("ATA_identify: Found drive%d with %d sectors\n", drive, (uint32_t) drive_sectors);

}



void ATA_read_sector(void *target_address, uint32_t LBA)
{
  __asm__ __volatile__ ("cli");

	ATA_wait_not_BSY();
	outportb(ATA_DRV_HD_REG, LBA_DRV_HD(LBA));
  outportb(ATA_ERR_FEAT_REG, 0);
	outportb(ATA_SCTR_COUNT_REG, 1);
	outportb(ATA_LBA_LOW_REG, LBA_LOW(LBA));
	outportb(ATA_LBA_MID_REG, LBA_MID(LBA));
	outportb(ATA_LBA_HIGH_REG, LBA_HIGH(LBA));
	outportb(ATA_STAT_CMD_REG, ATA_CMD_READ); // send the read command

	uint32_t *target = (uint32_t *) target_address;

	ATA_wait_not_BSY();
  ATA_wait_DRQ();
  ATA_wait_ERR();


	for(uint32_t i = 0; i < ATA_SECTOR_SIZE/sizeof(uint32_t); i++){
		target[i] = inportd(ATA_DATA_REG);
  }

  __asm__ __volatile__ ("sti");

}

void ATA_read_sectors(void *target_address, uint32_t LBA, uint32_t sector_count)
{
  for (uint32_t j = 0; j < sector_count; j++) {
    ATA_read_sector(target_address + (j * ATA_SECTOR_SIZE), LBA + j);
  }
}




void ATA_write_sector(void *target_address, uint32_t LBA)
{
  __asm__ __volatile__ ("cli");

	ATA_wait_not_BSY();
	outportb(ATA_DRV_HD_REG, LBA_DRV_HD(LBA) );
  outportb(ATA_ERR_FEAT_REG, 0);
	outportb(ATA_SCTR_COUNT_REG, 1);
	outportb(ATA_LBA_LOW_REG, LBA_LOW(LBA) );
	outportb(ATA_LBA_MID_REG, LBA_MID(LBA) );
	outportb(ATA_LBA_HIGH_REG, LBA_HIGH(LBA) );
	outportb(ATA_STAT_CMD_REG, ATA_CMD_WRITE); // send the read command

	uint32_t *target = (uint32_t*) target_address;

  ATA_wait_not_BSY();
	ATA_wait_DRQ();
  ATA_wait_ERR();

	for(uint32_t i = 0; i < ATA_SECTOR_SIZE/sizeof(uint32_t); i++)
		outportd(ATA_DATA_REG, target[i]);

  __asm__ __volatile__ ("sti");
}

void ATA_write_sectors(void *target_address, uint32_t LBA, uint32_t sector_count)
{
  for (uint32_t j = 0; j < sector_count; j++) {
    ATA_write_sector(target_address + (j * ATA_SECTOR_SIZE), LBA + j);
  }
}


/*
// do we really need this when we can get it in the ata identify call?
uint64_t ATA_drive_size(int drive_num, int *block_size)
{
  outportb(ATA_DRV_HD_REG, ATA_DRIVE_MASTER);

  ATA_wait_RDY();

  outportb(ATA_STAT_CMD_REG, 0x27); //READ NATIVE MAX ADDRESS EXT

  ATA_wait_not_BSY();

  uint64_t Max_LBA;
  Max_LBA =  (unsigned long long )inportb(ATA_LBA_LOW_REG);
  Max_LBA += (unsigned long long )inportb(ATA_LBA_MID_REG) <<8;
  Max_LBA += (unsigned long long )inportb(ATA_LBA_HIGH_REG) <<16;


  return Max_LBA;
}
*/


void ATA_wipe_disk(void)
{
	// well, first few hundred blocks
	char buf[512];
	memset(buf, 0, 512);
	ATA_write_sectors(buf, 0, 8*530);
}

void ATA_init(void)
{
	// really we should identify all possible disks, but for now...
  ATA_identify(0);
  ATA_identify(1);
}
