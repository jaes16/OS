#include <fat_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>

int fat_get_new_block(void)
{
  for(unsigned int i = fat_table_blocks+1; i < fat_total_blocks; i++){
    if (fat[i] == FAT_TABLE_FREE){
      fat[i] = FAT_TABLE_EOF;
			int disk_pos = (i * 4) / FAT_BLOCK_SIZE;
			fat_write_blocks(fat + (disk_pos * FAT_BLOCK_SIZE), disk_pos + 1, 1);
      return i;
    }
  }
  return -ENOSPC;
}


int fat_add_blocks(fat_dir_entry *dir_ent, int num_new_blocks)
{
	// see if we have enough free blocks
	if ((uint32_t)num_new_blocks > fat_free_blocks) return -ENOSPC;

	// get last block of file
  int cur_num = dir_ent->cluster_num;
  while(fat[cur_num] != FAT_TABLE_EOF) cur_num = fat[cur_num];

	int previous_last_block = cur_num;
	// add new blocks
	for(unsigned int i = fat_table_blocks+1; i < fat_total_blocks; i++){
		if (fat[i] == FAT_TABLE_FREE){
			fat[i] = FAT_TABLE_EOF;
			fat[cur_num] = i;
			cur_num = i;
			num_new_blocks--;

			// writeout updated fat
			int disk_pos = (i * 4) / FAT_BLOCK_SIZE;
			fat_write_blocks(fat + (disk_pos * FAT_BLOCK_SIZE), disk_pos + 1, 1);

			if (num_new_blocks == 0) break;
		}
	}

	// if for some reason we couldn't find enough blocks, our fat_free_blocks count is wrong and we're out of space
	if (num_new_blocks != 0) return -ENOSPC;

  return previous_last_block;
}


int fat_add_dir_ent(fat_dir_entry *parent, fat_dir_entry *new, uint32_t parent_block)
{
	// walk through all blocks of the parent to find an empty spot to put this directory entry
	int cur_num = parent->cluster_num;

	fat_dir_entry buf[fat_ents_per_blck()+1];
	memset(buf, 0, FAT_BLOCK_SIZE);

	int index = -1;

	while((uint32_t) cur_num != FAT_TABLE_EOF){

		fat_read_blocks(buf, cur_num, 1);
		// read through all possible entries of the block
		for (uint32_t i = 0; i < fat_ents_per_blck(); i++){
			// checking all the first characters of file names. if the first char == the value 0, then the dir_ent is empty
			if (buf[i].file_name[0] == 0){
				index = i;
				break;
			}
		}
		// if we didn't find an empty spot, look at next block
		if(index != -1) break;
		cur_num = fat[cur_num];
	}

	// didn't find an empty entry
	if (index == -1){
		cur_num = fat_add_blocks(parent, 1);
		if (cur_num < 0) return cur_num; // no space
		cur_num = fat[cur_num];
		index = 0;
		memset(buf, 0, FAT_BLOCK_SIZE);
	}

	memcpy(&buf[index], new, sizeof(fat_dir_entry));
	fat_write_blocks(buf, cur_num, 1);

	// update size of parent directory entry
	fat_read_blocks(buf, parent->cluster_num, 1);
	buf[0].file_size = parent->file_size;
	if(parent == fat_root_entry){
		// if parent is root, have to update '..' as well
		buf[1].file_size = parent->file_size;
		//
		if(fat_root_entry->cluster_num == fat_cur_entry->cluster_num) fat_cur_entry->file_size = parent->file_size;
		fat_write_blocks(buf, cur_num, 1);
		return 0;
	}
	else{

		fat_write_blocks(buf, cur_num, 1);
		// if parent is not root, gotta change its directory entry in the grandparent
		fat_read_blocks(buf, parent_block, 1);
		for (uint32_t i = 0; i < fat_ents_per_blck(); i++){
			// find parent directory entry
			if (strncmp(buf[i].file_name, parent->file_name, FAT_MAX_FILE_NAME) == 0){
				// copy and write out
				memcpy(&buf[i], parent, sizeof(fat_dir_entry));
				fat_write_blocks(buf, parent_block, 1);
				return 0;
			}
		}
		return 0;
	}
	return 0;
}

int fat_mkdir(char* path, uint32_t mode)
{
	/*
	Checking:
	Check path and file name
	Check if parent exists and is a directory
	Check if parent directory permissions are writable
	Check if there is a free block for a new directory
	Creation:
	Create two directory entries, '.': new directory, '..' the parent directory
	Add new directory entry to parent data, change parent data and grandp data abort if we can't get space
	*/

  // check path and file name
  fat_dir_entry *start = 0;
  char file_name[FAT_MAX_FILE_NAME];
  memset(file_name, 0, FAT_MAX_FILE_NAME);
  int base_loc = fat_check_path(path, start, file_name);
  if (base_loc < 0) return base_loc;

  // check parent
  fat_dir_entry *parent_ent;
  int parent_block;
  char parent_path[FAT_MAX_PATH_LEN+1];
  memset(parent_path, 0, FAT_MAX_PATH_LEN+1);
  if (base_loc == 0){ // only filename
    // either parent is root or current directory
    if (path[0] == '/' || fat_cur_entry->cluster_num == fat_root_entry->cluster_num){
			parent_ent = fat_root_entry;
			parent_block = fat_root_index;
		}
    else{
      parent_ent = fat_cur_entry;
      parent_block = fat_cur_ent_block;
    }
  }
  else { // find parent entry
    memcpy(parent_path, path, base_loc-1);
    fat_dir_entry parent;
    parent_ent = &parent;
    parent_block = fat_find_entry(parent_path, parent_ent, start);
    if (parent_block < 0) return parent_block;
		if ((parent_ent->file_attribute & FAT_FILE_ATTR_DIR) == 0) return -ENOTDIR;
		if (parent_ent->cluster_num == fat_cur_entry->cluster_num &&
			(strncmp(parent_ent->file_name, fat_cur_entry->file_name, FAT_MAX_PATH_LEN) == 0)) parent_ent = fat_cur_entry;
  }

	// check parent directory permissions
	if((parent_ent->file_attribute & FAT_PERM_OWN_WRITE) == 0) return -EACCES;

  // see if the file already exists, or some other error
  fat_dir_entry d_entry;
  int block_num = fat_find_entry(file_name, &d_entry, parent_ent);
  if (block_num >= 0) return -EEXIST;
  else if (block_num != -ENOENT) return block_num;

  // checking if there is enough free space
  // see if there is a free block
  block_num = fat_get_new_block();
  if (block_num < 0) return -ENOSPC;


  /////////// creation: let there be data ////////////

  fat_dir_entry buf[fat_ents_per_blck()+1];
  memset(buf, 0, FAT_BLOCK_SIZE);

  // create two directory entries and write out to appropriate block
	memset(&d_entry, 0, sizeof(fat_dir_entry));
  d_entry.file_name[0] = '.';
  d_entry.file_attribute = mode | FAT_FILE_ATTR_DIR;
  d_entry.creation_time = timer_ticks;
  d_entry.file_size = 2 * sizeof(fat_dir_entry);
  d_entry.cluster_num = block_num;
	memcpy(&buf[0], &d_entry, sizeof(fat_dir_entry));
  // ".." direntry
	parent_ent->file_size += sizeof(fat_dir_entry);
  memcpy(&buf[1], parent_ent, sizeof(fat_dir_entry));
  memset(buf[1].file_name, 0, FAT_MAX_FILE_NAME);
	buf[1].file_name[0] = '.';
	buf[1].file_name[1] = '.';

  // add dir_ent to parent directory block
	memcpy(d_entry.file_name, file_name, FAT_MAX_FILE_NAME);
	if (fat_add_dir_ent(parent_ent, &d_entry, parent_block) < 0) {
		// no space for parent, undo and return error
		fat[block_num] = FAT_TABLE_FREE;
		parent_ent->file_size -= sizeof(fat_dir_entry);
		return -ENOSPC;
	}
	// otherwise, writeout buffer
	fat_write_blocks(buf, block_num, 1);

  return 0;
}


int fat_mknod(char *path, uint32_t mode)
{
	/*
	Checking:
	Check path and file name
	Check if parent exists and is a directory
	Check if parent directory permissions are writable
	Check if there is a free block for a new file
	Creation:
	Zero out block
	Add new directory entry to parent data, change parent data and grandp data abort if we can't get space
	*/

	// Checks:
	// check path and file name
  fat_dir_entry *start = 0;
  char file_name[FAT_MAX_FILE_NAME];
  memset(file_name, 0, FAT_MAX_FILE_NAME);
  int base_loc = fat_check_path(path, start, file_name);
  if (base_loc < 0) return base_loc;

  // check parent
  fat_dir_entry *parent_ent;
  int parent_block;
  char parent_path[FAT_MAX_PATH_LEN+1];
  memset(parent_path, 0, FAT_MAX_PATH_LEN+1);
  if (base_loc == 0){ // only filename
    // either parent is root or current directory
    if (path[0] == '/' || fat_cur_entry->cluster_num == fat_root_entry->cluster_num){
			parent_ent = fat_root_entry;
			parent_block = fat_root_index;
		}
    else{
      parent_ent = fat_cur_entry;
      parent_block = fat_cur_ent_block;
    }
  }
  else { // find parent entry
    memcpy(parent_path, path, base_loc-1);
    fat_dir_entry parent;
    parent_ent = &parent;
    parent_block = fat_find_entry(parent_path, parent_ent, start);
    if (parent_block < 0) return parent_block;
		if ((parent_ent->file_attribute & FAT_FILE_ATTR_DIR) == 0) return -ENOTDIR;
		if (parent_ent->cluster_num == fat_cur_entry->cluster_num &&
			(strncmp(parent_ent->file_name, fat_cur_entry->file_name, FAT_MAX_PATH_LEN) == 0)) parent_ent = fat_cur_entry;
  }

	// check parent directory permissions
	if((parent_ent->file_attribute & FAT_PERM_OWN_WRITE) == 0) return -EACCES;

  // see if the file already exists, or some other error
  fat_dir_entry d_entry;
  int block_num = fat_find_entry(file_name, &d_entry, parent_ent);
  if (block_num >= 0) return -EEXIST;
  else if (block_num != -ENOENT) return block_num;

  // checking if there is enough free space
  // see if there is a free block
  block_num = fat_get_new_block();
  if (block_num < 0) return -ENOSPC;


	/////////// creation: let there be data ////////////

	d_entry.file_attribute = mode;
	d_entry.creation_time = timer_ticks;
	d_entry.file_size = 0;
	d_entry.cluster_num = block_num;
	memcpy(d_entry.file_name, file_name, FAT_MAX_FILE_NAME);

	// add dir_ent to parent directory block
	parent_ent->file_size += sizeof(fat_dir_entry);
	if (fat_add_dir_ent(parent_ent, &d_entry, parent_block) < 0) {
		// no space for parent, undo and return error
		fat[block_num] = FAT_TABLE_FREE;
		parent_ent->file_size -= sizeof(fat_dir_entry);
		return -ENOSPC;
	}

	// otherwise, zero out our first block
	char buf[FAT_BLOCK_SIZE];
	memset(buf, 0, FAT_BLOCK_SIZE);
	fat_write_blocks(buf, block_num, 1);

	return 0;
}
