#ifndef __EXT_FS_H
#define __EXT_FS_H

/*
  Loosely based on ext2 design
*/

#include <ata.h>

#define EXT_BLOCK_SIZE          4096
#define EXT_INODE_SIZE          96


#define EXT_SCTR_PER_BLCK       FAT_BLOCK_SIZE / ATA_SECTOR_SIZE


/*
  First block is superblock: 4096 bytes
  Then the block bitmap: 4096 bytes: keeps track of 4096*8 blocks = 32768 * 4096 = keeps track of 2^27 bytes
  Then the inode bitmap: 4096 bytes: again, 2^27 possible inodes
  Then the inode table: 32 inodes per block

  For now, no groups and group descriptors
*/



typedef struct ext_superblock
{
  uint32_t total_inodes;
  uint32_t total_blocks;
  uint32_t superuser_blocks;
  uint32_t unallocated_blocks;
  uint32_t unallocated_inodes;
  uint32_t superblock_index;
  uint32_t block_size_kb;
  uint32_t fragment_size_kb;
  uint32_t blocks_per_group;
  uint32_t fragments_per_group;
  uint32_t inodes_per_group
  uint32_t mount_time;
  uint32_t written_time; // 13 * 32 = 416
  uint16_t num_mounts;
  uint16_t max_mounts;
  uint16_t fs_state;
  uint16_t err_handling;
  uint16_t version_low; // 5 * 16 = 80 + 416 = 496
  uint32_t const_check_time;
  uint32_t os_id;
  uint32_t version_high; // 3 * 32 = 96 + 496 = 592
  uint16_t uid;
  uint16_t gid; // 2 *16 = 32 + 592 = 624
}

typedef struct ext_inode // size: 128 bytes. Roughly following simplified ext2 structure.
{
  uint16_t mode; // permission bits
  uint16_t uid; // owner user id
  uint32_t size; // size in bytes
  uint32_t time; // last access time
  uint32_t ctime; // creation time
  uint32_t mtime; // last modified time
  uint32_t dtime; // deletion time
  uint16_t gid; // owning group id
  uint16_t links_count; // number of hard links to this file
  uint32_t block_count; // number of blocks allocated for this file
  uint32_t flags;
  uint32_t block[15]; // pointers to blocks (absolute locations). pointer 0 - 11 direct, 12 indirect, 13 doubly indirect, 14 triply indirect
  uint32_t reserved[8];
} ext_inode;


extern int ext_read_blocks(void *buf, size_t num_blocks, size_t offset);
extern int ext_write_blocks(void *buf, size_t num_blocks, size_t offset);












#endif //__EXT_FS_H
