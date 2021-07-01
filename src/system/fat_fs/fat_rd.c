#include <fat_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>

// return offset
int fat_readdir(char *path, void *buf, size_t buf_size, size_t offset)
{
	/*
	Checking:
	Check path and file name
	Check if directory exists
	Check if directory is readable
	Reading:
	Read from offset and write into buf a buf_size amount
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

	// check if it is a directory
	if ((d_entry.file_attribute & FAT_FILE_ATTR_DIR) == 0) return -ENOTDIR;
	// check read permissions
	if ((d_entry.file_attribute & FAT_PERM_OWN_READ) == 0) return -EACCES;

	///////// Reading! //////////
	size_t ret_offset = offset;

	int cur_block = d_entry.cluster_num;
	// find index to read from
	while (offset >= FAT_BLOCK_SIZE){
		if ((uint32_t) cur_block == FAT_TABLE_EOF) return offset;
		cur_block = fat[cur_block];
		offset -= FAT_BLOCK_SIZE;
	}

	size_t size_written = 0;
	fat_dir_entry entries[fat_ents_per_blck()+1];
	// keep writing to buf till we run out of space or out of material
	while ((uint32_t) cur_block != FAT_TABLE_EOF){
		fat_read_blocks(entries, cur_block, 1);
		for (uint32_t i = 0; i < fat_ents_per_blck(); i++){
			if (offset > 0) offset -= sizeof(fat_dir_entry); // move to remaining offset in block
			else if (size_written+sizeof(fat_dir_entry) > buf_size) return ret_offset; // not enough space in buf
			else if (entries[i].file_name[0] != 0){ // found a valid entry to write out
				if((uint32_t) cur_block == d_entry.cluster_num && i == 1){
					// get parent directory entry
					char buf2[FAT_BLOCK_SIZE];
					fat_read_blocks(buf2, entries[i].cluster_num, 1);
					memcpy(buf+size_written, buf2, sizeof(fat_dir_entry));
					((char *)buf+size_written)[1] = '.';
				}
				else memcpy(buf+size_written, &entries[i], sizeof(fat_dir_entry));
				size_written += sizeof(fat_dir_entry);
			}
			ret_offset += sizeof(fat_dir_entry);
		}
		cur_block = fat[cur_block];
	}
  return ret_offset;
}



int fat_read (char *path, void *buf, size_t buf_size, size_t offset)
{
	/*
	Checking:
	Check path and file name
	Check if file exists
	Check if file is readable
	Reading:
	Read from offset and write into buf a buf_size amount
	Return size written
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
	if ((d_entry.file_attribute & FAT_PERM_OWN_READ) == 0) return -EACCES;
	// check if offset is beyond file size
	if (d_entry.file_size <= offset) return 0;

	///////// Reading! //////////

	int cur_block = d_entry.cluster_num;
	// find index to read from
	while (offset >= FAT_BLOCK_SIZE){
		if ((uint32_t) cur_block == FAT_TABLE_EOF) return offset;
		cur_block = fat[cur_block];
		offset -= FAT_BLOCK_SIZE;
	}

	size_t size_written = 0;
	char disk_block[FAT_BLOCK_SIZE];
	// keep writing to buf till we run out of space or out of material
	while ((uint32_t) cur_block != FAT_TABLE_EOF){
		fat_read_blocks(disk_block, cur_block, 1);

		// read from offset, either the rest of the block, or as much as buf can take
		size_t read_size = FAT_BLOCK_SIZE - offset;
		if (buf_size < read_size) read_size = buf_size;
		memcpy(buf+size_written, disk_block+offset, read_size);
		size_written += read_size;
		offset = 0;
		if(size_written >= buf_size) return size_written;
		cur_block = fat[cur_block];
	}
	return size_written;
}
