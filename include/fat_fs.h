#ifndef _FAT_FS_H
#define _FAT_FS_H

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>

#define FAT_BLOCK_SIZE          4096
#define FAT_D_ENTRY_SIZE        64

#define FAT_SCTR_PER_BLCK       (FAT_BLOCK_SIZE / ATA_SECTOR_SIZE)
#define FAT_D_ENT_PER_BLCK      (FAT_BLOCK_SIZE / FAT_D_ENTRY_SIZE)


#define FAT_MAX_FILE_NAME       36
#define FAT_MAX_PATH_LEN        4095

#define FAT_TABLE_FREE          0xffffffff
#define FAT_TABLE_EOF           0xfffffffe

#define FAT_FILE_ATTR_PERM      0x01ff // permission masks below
#define FAT_FILE_ATTR_HIDDEN    0x0200
#define FAT_FILE_ATTR_SYS       0x0400
#define FAT_FILE_ATTR_VOL       0x0800
#define FAT_FILE_ATTR_DIR       0x1000
#define FAT_FILE_ATTR_ARCHIVE   0x2000
#define FAT_FILE_ATTR_RESERVE0  0x4000
#define FAT_FILE_ATTR_RESERVE1  0x8000
                                            // Owner Grp Other
                                            //   rwx rwx rwx
#define FAT_PERM_MASK           0x1ff       // 0b111 111 111
#define FAT_PERM_OWN_READ       0x100       // 0b100 000 000
#define FAT_PERM_OWN_WRITE      0x080       // 0b010 000 000
#define FAT_PERM_OWN_EXEC       0x040       // 0b001 000 000
#define FAT_PERM_GRP_READ       0x020       // 0b000 100 000
#define FAT_PERM_GRP_WRITE      0x010       // 0b000 010 000
#define FAT_PERM_GRP_EXEC       0x008       // 0b000 001 000
#define FAT_PERM_OTH_READ       0x004       // 0b000 000 100
#define FAT_PERM_OTH_WRITE      0x002       // 0b000 000 010
#define FAT_PERM_OTH_EXEC       0x001       // 0b000 000 001

/*
*   FATFS-ish: This file system is loosely based on the FATFS design structure, but doesn't actually fit the specifications
*
*   We zero out data that is deleted for implementation purposes, even though it has performance hits. Besides, it has
*   security benefits
*/
typedef struct fat_superblock
{
  uint32_t fat_location;
  uint32_t fat_table_blocks;
  uint32_t data_location; //root dir first cluster location
  uint32_t data_blocks;

  char pad[4080];
} fat_superblock;


typedef struct fat_dir_entry // size: 64 bytes. Loosely based on FAT32 structure, but, file names are max 36 chars
{
  char file_name[FAT_MAX_FILE_NAME];
  int file_attribute;
  char reserved;
  char creation_time_milisec;
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t access_date;
  uint16_t write_time;
  uint16_t write_date;
  uint32_t cluster_num;
  uint32_t file_size;
  uint32_t num_blocks;
} fat_dir_entry;


// based loosely on Linux stat struct. Somethings, like the inode, don't fit well with fat, but it serves its purpose for now
typedef struct fat_stat
{
  uint32_t st_dev;         /* ID of device containing file */
  uint32_t st_ino;         /* Inode number */
  uint32_t st_mode;        /* File type and mode */
  uint32_t st_nlink;       /* Number of hard links */
  uint32_t st_uid;         /* User ID of owner */
  uint32_t st_gid;         /* Group ID of owner */
  uint32_t st_rdev;        /* Device ID (if special file) */
  uint32_t st_size;        /* Total size, in bytes */
  uint32_t st_blksize;     /* Block size for filesystem I/O */
  uint32_t st_blocks;      /* Number of 512B blocks allocated */

  uint32_t st_atim;  /* Time of last access */
  uint32_t st_mtim;  /* Time of last modification */
  uint32_t st_ctim;  /* Time of last status change */

} fat_stat;

// variables
extern fat_superblock *fat_sb;
extern uint32_t *fat; // end of physical memory map

extern fat_dir_entry *fat_root_entry;
extern fat_dir_entry *fat_cur_entry;

extern char fat_cur_path[FAT_MAX_PATH_LEN];
extern uint32_t fat_cur_ent_block;
extern uint32_t fat_root_index;

extern uint64_t fat_total_blocks;
extern uint64_t fat_data_blocks;
extern uint64_t fat_data_loc;
extern uint64_t fat_table_blocks;

/////// fat_init.c ////////
extern char * basename(char *path);
extern int dirname(char *path);
extern int fat_read_blocks(void *buf, size_t offset, size_t num_blocks);
extern int fat_write_blocks(void *buf, size_t offset, size_t num_blocks);
extern uint32_t fat_init(uint32_t fat_address);
extern size_t sizeof_fat(void);
extern uint32_t fat_ents_per_blck(void);


/////// fat_attr.c ////////
/*  in:
    out:    if dir entry is found, saves it in d_entry
    error:  -ENOENT if no dir entry is found
            -ENAMETOOLONG if file name too long
            -ENOTDIR if a directory in the path is not a directory
*/
extern int fat_find_entry(char *path, fat_dir_entry *d_entry, fat_dir_entry *start);
extern int fat_check_path(char *path, fat_dir_entry *start, char *file_name);
extern int fat_getattr(char *path, fat_dir_entry *d_entry);
extern int fat_access(char *path, uint32_t mask);

/////// fat_mk.c ////////
extern int fat_mkdir(char* path, uint32_t mode);

/////// fat_rd.c ////////
extern int fat_readdir(char* path, void *buf, size_t buf_size, size_t offset);

/////// fat_rm.c ////////
extern int fat_rmdir(char *path);

#endif
