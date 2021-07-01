#include <fat_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>


int fat_truncate (char *path, size_t size)
{
	/*
	Checking:
	Check path and file name
	Check if file exists
	Check if file is writable
	If we're adding blocks, see if we have enough blocks
	Trucating:
	See if we need to decrease or increase
	If we need to decrease, free up blocks and zero them out in disk
	If we need to increase, allocate more blocks
	*/

  // check path and file name
  fat_dir_entry *start = 0;
  char file_name[FAT_MAX_FILE_NAME];
  memset(file_name, 0, FAT_MAX_FILE_NAME);
  int base_loc = fat_check_path(path, start, file_name);
  if (base_loc < 0) return base_loc;

	// check if file exists
  fat_dir_entry d_entry;
  int block_num = fat_find_entry(path, &d_entry, start);
  if (block_num < 0) return block_num;

	// check if it is a file
	if ((d_entry.file_attribute & FAT_FILE_ATTR_DIR) != 0) return -EISDIR;
	// check permissions
	if ((d_entry.file_attribute & FAT_PERM_OWN_WRITE) == 0) return -EACCES;


	///////// Truncate! //////////

	uint32_t cur_block = d_entry.cluster_num;


	// check if offset is beyond file size
	if (d_entry.file_size == size) return size;
	else if(d_entry.file_size > size){
		// decrease file size
		char buf[FAT_BLOCK_SIZE];

		// find the last block for this size
		while(size >= FAT_BLOCK_SIZE){
			cur_block = fat[cur_block];
			size -= FAT_BLOCK_SIZE;
		}
		int temp = 0;
		if (size != 0){ // truncate within block
			// zero out any data after size
			fat_read_blocks(buf, cur_block, 1);
			memset(buf+size, 0, FAT_BLOCK_SIZE - size);
			fat_write_blocks(buf, cur_block, 1);
			size = 0;
			temp = fat[cur_block];
			fat[cur_block] = FAT_TABLE_FREE;
			cur_block = temp;
		}

		memset(buf, 0, FAT_BLOCK_SIZE);
		// finish zeroing out all the blocks
		while(cur_block != FAT_TABLE_EOF){
			if(cur_block == d_entry.cluster_num) break;
			fat_write_blocks(buf, cur_block, 1);
			temp = fat[cur_block];
			fat[cur_block] = FAT_TABLE_FREE;
			cur_block = temp;
		}

	}
	// if we need just one block, we already have it.
	else if (size > FAT_BLOCK_SIZE){
		// increase file size

		// find how much more size we need to allocate
		while(fat[cur_block] != FAT_TABLE_EOF){
			size -= FAT_BLOCK_SIZE;
			cur_block = fat[cur_block];
		}

		// see how many blocks we need
		int num_new_blocks = ((size - FAT_BLOCK_SIZE) / FAT_BLOCK_SIZE);
		if (size % FAT_BLOCK_SIZE != 0) num_new_blocks++;

		// allocate blocks
		int res = fat_add_blocks(&d_entry, num_new_blocks);
		if (res < 0) return res; // not enough space to allocate

	}

	// update file size in directory entry
	fat_dir_entry buf[fat_ents_per_blck()+1];
	fat_read_blocks(buf, block_num, 1);
	for(uint32_t i = 0; i < fat_ents_per_blck(); i++){
		if (buf[i].cluster_num == d_entry.cluster_num && strncmp(buf[i].file_name,d_entry.file_name,FAT_MAX_FILE_NAME)==0){
			buf[i].file_size = size;
			break;
		}
	}
	fat_write_blocks(buf, block_num, 1);


	return size;
}
