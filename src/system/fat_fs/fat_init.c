#include <fat_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>

fat_superblock *fat_sb = 0;
uint32_t *fat = 0;

fat_dir_entry *fat_root_entry;
fat_dir_entry *fat_cur_entry;
uint32_t fat_root_index;
uint32_t fat_cur_ent_block;

char fat_cur_path[FAT_MAX_PATH_LEN];
uint32_t fat_cur_dir_entry;

uint64_t fat_total_blocks;
uint64_t fat_data_blocks;
uint64_t fat_data_loc;
uint64_t fat_table_blocks;

// returns a pointer to the string after '/'
char * basename(char *path)
{
  int pathlen = strlen(path);
  if(pathlen == 0) return NULL;
  else{
    for(int i = pathlen - 1; i >= 0; i--){
      if(path[i] == '/') return path+i+1;
    }
    return path;
  }
}

// returns the index of the last '/', and -1 if there isn't
int dirname(char *path)
{
  int pathlen = strlen(path);

  if(pathlen == 0) return -1;
  else{
    for(int i = pathlen - 1; i >= 0; i --){
      if(path[i] == '/') return i;
    }
    return -1;
  }
}

int fat_read_blocks(void *buf, size_t offset, size_t num_blocks)
{
  ATA_read_sectors(buf, offset * FAT_SCTR_PER_BLCK, num_blocks * FAT_SCTR_PER_BLCK);
  return 0;
}
int fat_write_blocks(void *buf, size_t offset, size_t num_blocks)
{
  ATA_write_sectors(buf, offset * FAT_SCTR_PER_BLCK, num_blocks * FAT_SCTR_PER_BLCK);
  return 0;
}


size_t sizeof_fat(void){return fat_sb->fat_table_blocks * FAT_BLOCK_SIZE;}

uint32_t fat_ents_per_blck(void){return FAT_BLOCK_SIZE/sizeof(fat_dir_entry);};


// For testing purposes
void fat_init_disk(void)
{
  printf("\nDisk not recognized as FAT, initializing disk...\n");
  // initiate file allocation table
  // set all blocks as free
  memsetd(fat, FAT_TABLE_FREE, fat_total_blocks);
  // set superblock, root, and fat blocks as endoffile
  memsetd(fat, FAT_TABLE_EOF, fat_table_blocks + 2);

  // write out fat
  fat_write_blocks(fat, 1, 1);
  fat_write_blocks(fat+FAT_BLOCK_SIZE, 2, fat_table_blocks-1);
  // write out superblock
  memset(fat_sb, 0, FAT_BLOCK_SIZE);
  fat_sb->fat_location = FAT_BLOCK_SIZE;
  fat_sb->fat_table_blocks = fat_table_blocks;
  fat_sb->data_location = FAT_BLOCK_SIZE * (1 + fat_sb->fat_table_blocks);
  fat_sb->data_blocks = fat_data_blocks;
  fat_write_blocks(fat_sb, 0, 1);

  // write out root directory
  fat_dir_entry root_buf[FAT_D_ENT_PER_BLCK + 1];
  memset(root_buf, 0, FAT_BLOCK_SIZE);
  fat_root_entry->file_name[0] = '.';
  fat_root_entry->file_attribute = FAT_PERM_OWN_READ | FAT_PERM_OWN_WRITE | FAT_PERM_OWN_EXEC | FAT_PERM_GRP_READ | FAT_PERM_GRP_EXEC | FAT_PERM_OTH_READ | FAT_PERM_OTH_EXEC;
  fat_root_entry->file_attribute |= FAT_FILE_ATTR_DIR;
  fat_root_entry->file_attribute |= FAT_FILE_ATTR_SYS;
	fat_root_entry->cluster_num = fat_root_index;
  fat_root_entry->file_size = sizeof(fat_dir_entry) * 2;
  memcpy(&root_buf[0], fat_root_entry, sizeof(fat_dir_entry));
  memcpy(&root_buf[1], fat_root_entry, sizeof(fat_dir_entry));
  root_buf[1].file_name[1] = '.';
  fat_write_blocks(root_buf, fat_data_loc / FAT_BLOCK_SIZE, 1);

	// current directory
	memcpy(fat_cur_entry, fat_root_entry, sizeof(fat_dir_entry));

}

uint32_t fat_init(uint32_t fat_address)
{
  // set up fat in memory. Currently we're limiting disk size, so if the user runs on larger disk sizes, we can't promise smooth runs
	fat_root_entry = (fat_dir_entry *) fat_address;
	fat_cur_entry = (fat_dir_entry *) (fat_address + sizeof(fat_dir_entry));
  fat_sb = (fat_superblock *) (fat_address + (2 * sizeof(fat_dir_entry)));
  fat = (uint32_t *) (fat_address + (2 * sizeof(fat_dir_entry)) + FAT_BLOCK_SIZE);


  memset(fat_root_entry, 0, sizeof(fat_dir_entry));

  fat_total_blocks = ATA_SECTOR_COUNT() / FAT_SCTR_PER_BLCK;
  fat_table_blocks = ((fat_total_blocks - 1) / (FAT_BLOCK_SIZE/sizeof(uint32_t))) + 1;
  fat_data_loc = FAT_BLOCK_SIZE * (1 + fat_table_blocks);
  fat_data_blocks = (fat_total_blocks - 1) - fat_table_blocks;

  fat_root_index = fat_data_loc / FAT_BLOCK_SIZE;
	fat_cur_ent_block = fat_root_index;


  // check disk superblock
  fat_read_blocks(fat_sb, 0, 1);
  // check if valid superblock
  if( (fat_sb->fat_location == FAT_BLOCK_SIZE) && (fat_sb->fat_table_blocks == fat_table_blocks) && (fat_sb->data_location == FAT_BLOCK_SIZE*(fat_table_blocks+1)) ) {
    // if valid, read in fat
    fat_read_blocks(fat, 1, fat_table_blocks);

		// read in root directory
    char buf[512];
    ATA_read_sectors(buf, fat_data_loc / ATA_SECTOR_SIZE, 1);
    memcpy(fat_root_entry, buf, sizeof(fat_dir_entry));
		memcpy(fat_cur_entry, buf, sizeof(fat_dir_entry));
  }
  // if not valid, initialize disk (for testing purposes. Later on we should just halt)
  else fat_init_disk();

	memset(fat_cur_path, 0, FAT_MAX_PATH_LEN);
	fat_cur_path[0] = '/';

  return fat_address + (fat_table_blocks * FAT_BLOCK_SIZE);
}
