#include <ext_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>


uint64_t ext_max_blocks;
ext_superblock *sb;
ext_groupblock *ext_groups;
uint8_t *ext_block_bitmap;
uint8_t *ext_inode_bitmap;
ext_inode *ext_inode_table;

// kb, mb, gb, tb
// 10, 20, 30, 40, 50, 60

int ext_read_blocks(void *buf, size_t num_blocks, size_t offset)
{
  ATA_PIO_read_sectors(buf, offset * EXT_SCTR_PER_BLCK, num_blocks * EXT_SCTR_PER_BLCK);
  return 0;
}

int ext_write_blocks(void *buf, size_t num_blocks, size_t offset)
{
  ATA_PIO_write_sectors(buf, offset * EXT_SCTR_PER_BLCK, num_blocks * EXT_SCTR_PER_BLCK);
  return 0;
}

void ext_init_disk(void)
{

}

int ext_init(uint32_t pmem_map_end, uint32_t disk_high_kb, uint32_t disk_low_kb)
{

  ext_max_blocks = ATA_SECTOR_COUNT() / EXT_SCTR_PER_BLCK;
  ext_block_bitmap = (uint8_t *) pmem_map_end;
  ext_inode_bitmap = (uint8_t *) (ext_block_bitmap + EXT_BLOCK_SIZE);
  ext_inode_table = (ext_inode *) (ext_inode_bitmap + EXT_BLOCK_SIZE);
  // set up block bit table, inode bit table, inode table
}
