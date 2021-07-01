#include <fat_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>


int fat_write (char *path, void *buf, size_t buf_size, size_t offset)
{
	/*
	Checking:
	Check path and file name
	Check if file exists
	Check if file is writable
	Writing:
	Write from buf to offset in block
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
	if ((d_entry.file_attribute & FAT_PERM_OWN_WRITE) == 0) return -EACCES;
	// check if offset is beyond file size
	if (d_entry.file_size <= offset) return 0;

	///////// Writing! //////////

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

		// write from buf to disk block, either the rest of buf, or as much as block can take
		size_t write_size = FAT_BLOCK_SIZE - offset;
		if (buf_size < write_size) write_size = buf_size;
		memcpy(disk_block+offset, buf+size_written, write_size);

		// update the size we've written, and now we can ignore offset
		size_written += write_size;
		offset = 0;

		fat_write_blocks(disk_block, cur_block, 1);

		// if we've written all we can, finish, otherwise get next block
		if(size_written >= buf_size) break;
		cur_block = fat[cur_block];
	}

	return size_written;
}
