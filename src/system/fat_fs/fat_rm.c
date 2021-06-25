#include <fat_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <ata.h>


int fat_free_block(int block_num)
{
	int disk_pos = (block_num * 4) / FAT_BLOCK_SIZE;
	fat[block_num] = FAT_TABLE_FREE;
	fat_write_blocks(fat + (disk_pos * FAT_BLOCK_SIZE), disk_pos + 1, 1);
	return 0;
}


int fat_remove_dir_ent(fat_dir_entry *parent, fat_dir_entry *target, uint32_t target_loc, uint32_t parent_block)
{
	fat_dir_entry buf[fat_ents_per_blck()+1];
	memset(buf, 0, FAT_BLOCK_SIZE);
	fat_read_blocks(buf, target_loc, 1);

	for(uint32_t i = 0; i < fat_ents_per_blck(); i++){
		if (buf[i].cluster_num == target->cluster_num && buf[i].file_size == target->file_size){
			memset(&buf[i], 0, sizeof(fat_dir_entry));
			break;
		}
	}

	fat_write_blocks(buf, target_loc, 1);

	// update parent size
	fat_read_blocks(buf, parent->cluster_num, 1);
	buf[0].file_size = parent->file_size;
	if(parent == fat_root_entry){
		buf[1].file_size = parent->file_size;
		fat_write_blocks(buf, parent->cluster_num, 1);
	}
	else{
		fat_write_blocks(buf, parent->cluster_num, 1);
		fat_read_blocks(buf, parent_block, 1);
		for(uint32_t i = 0; i < fat_ents_per_blck(); i++){
			if (buf[i].cluster_num == parent->cluster_num && buf[i].file_size == parent->file_size){
				memset(&buf[i], 0, sizeof(fat_dir_entry));
				break;
			}
		}
		fat_write_blocks(buf,parent_block, 1);
	}

	return 0;
}



int fat_rmdir (char *path)
{
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

	// check if file exists
  fat_dir_entry d_entry;
  int block_num = fat_find_entry(file_name, &d_entry, parent_ent);
  if (block_num < 0) return block_num;

	// check if directory
	if ((d_entry.file_attribute & FAT_FILE_ATTR_DIR) == 0) return -ENOTDIR;
	// check if we have write permissions
	if ((d_entry.file_attribute & FAT_PERM_OWN_WRITE) == 0) return -EACCES;
	// check if there are any files/directories in this directory other than '.' and '..'
	if (d_entry.file_size > (sizeof(fat_dir_entry) * 2)) return -ENOTEMPTY;


	/////////// removal:  ////////////
	// empty out all the blocks and free them up in the fat
	uint32_t cluster = d_entry.cluster_num;
	char buf[FAT_BLOCK_SIZE];
	memset(buf, 0, FAT_BLOCK_SIZE);
	while(cluster != FAT_TABLE_EOF){
		int temp = fat[cluster];
		fat_write_blocks(buf, cluster, 1);
		fat_free_block(cluster);
		cluster = temp;
	}

	// remove directory entry in parent directory and update parent directory size
	parent_ent->file_size -= sizeof(fat_dir_entry);
	fat_remove_dir_ent(parent_ent, &d_entry, block_num, parent_block);

	return 0;
}
