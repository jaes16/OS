#include <fat_fs.h>

#include <system.h>

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

int fat_permission(char attribute, int mask){
  mask = mask & FAT_PERM_MASK;
  if ((mask & attribute) == mask) return 0;
  else return -EACCES;
}

// saves direntry in pointer and returns the block it was found in, or returns error code
// not yet supporting symlinks
int fat_find_entry(char *path, fat_dir_entry *d_entry, fat_dir_entry *start)
{
	if (path[0] == '/'){ //root
		if (path[1] == 0){
			memcpy(d_entry, fat_root_entry, sizeof(fat_dir_entry));
			return fat_root_index;
		}
	}

  // find first '/'
  char *comp = path;
	if (path[0] == '/') comp++;
  char *next_path;
  uint32_t comp_len;
  fat_dir_entry *cur = start;

  fat_dir_entry block_buf[FAT_D_ENT_PER_BLCK + 1];
  uint32_t block_num = cur->cluster_num;

  // go through each component in the path to see if each exists
  fat_find_entry_loop:;

  // isolate the next directory/file name
  next_path = index(comp, '/');
  if (next_path == NULL) comp_len = strlen(comp);
  else comp_len = next_path - comp;
  if(comp_len > FAT_MAX_FILE_NAME) return -ENAMETOOLONG;

  // go through all the blocks of this directory
  while(block_num != FAT_TABLE_EOF){
    // read in block
    memset(block_buf, 0, FAT_BLOCK_SIZE);
    fat_read_blocks(block_buf, block_num, 1);

    // walkthrough each directory entry of block to find
    for(int i = 0; i <= FAT_D_ENT_PER_BLCK; i++){
      if((strnlen(block_buf[i].file_name, FAT_MAX_FILE_NAME) == comp_len) && (strncmp(block_buf[i].file_name, comp, comp_len) == 0)) {
        // found directory entry, return or move on

        if (next_path == NULL){ // this is the last component of path
          memcpy(d_entry, &block_buf[i], FAT_D_ENTRY_SIZE);
          return block_num;
        }
        else { // this isn't the last component of path
          // check if this component is a directory, otherwise this path is invalid
          if((block_buf[i].file_attribute & FAT_FILE_ATTR_DIR) == 0) return -ENOTDIR;
          comp = next_path;
					if (comp[0] == '/') comp++;
          block_num = block_buf[i].cluster_num;
          goto fat_find_entry_loop; // loop back to check next component of path
        }
        break;
      }
    }
    // didn't find the appropriate directory entry in this block, check next
    block_num = fat[block_num];
  }

  // didn't find the appropriate directory entry in any of the blocks
  return -ENOENT;
}


// checks path and file name length, and finds which directory entry to start from. returns where "/" is, -1 if non-existant
int fat_check_path(char *path, fat_dir_entry *start, char *file_name)
{
  // check if path length is within limits
  uint32_t path_len = strnlen(path, FAT_MAX_PATH_LEN+1);
  if (path_len == 0) return -ENOENT;
  if (path_len > FAT_MAX_PATH_LEN) return -ENAMETOOLONG;

	// see if path starts from current directory or from root
  if (path[0] == '/'){
		memcpy(start, fat_root_entry, sizeof(fat_dir_entry));
		if (path[1] == 0) return 0;
	}
	else memcpy(start, fat_cur_entry, sizeof(fat_dir_entry));


  // get file name length
  int base_loc = dirname(path) + 1;
  uint32_t file_name_len = path_len;
  if (base_loc != 0) file_name_len -= base_loc;

  // check file name length < max file name
  if (file_name_len > FAT_MAX_FILE_NAME) return -ENAMETOOLONG;

  // get file name if wanted
  if (file_name != NULL) memcpy(file_name, path+base_loc, file_name_len);

  return base_loc;
}



int fat_getattr(char *path, fat_dir_entry *d_ent)
{
  // check path and file name shenanigans
  fat_dir_entry *start = 0;
  int base_loc = fat_check_path(path, start, NULL);
  if (base_loc < 0) return base_loc;

  // check if file exists
  int block_num = fat_find_entry(path, d_ent, start);
  if (block_num < 0) return block_num; // if blocknum < 0, there's an error, so return it.

  // fill in appropriate stats
	/*
  memset(st_buf, 0, sizeof(fat_stat));
  st_buf->st_mode = d_ent->file_attribute;
  st_buf->st_size = d_ent->file_size;
  int num_blocks = 0;
  uint32_t b = d_ent->cluster_num;
  while(b != FAT_TABLE_EOF){num_blocks++; b = fat[b];}
  st_buf->st_blocks = num_blocks;
  st_buf->st_atim = d_ent->access_date;
  st_buf->st_mtim = d_ent->write_time;
  st_buf->st_ctim = d_ent->creation_time;
	*/

  return 0;
}



int fat_access(char *path, uint32_t mask)
{
  // check path and file name
  fat_dir_entry *start = 0;
  int base_loc = fat_check_path(path, start, NULL);
  if (base_loc < 0) return base_loc;

  // check if file exists
  fat_dir_entry d_entry;
  int block_num = fat_find_entry(path, &d_entry, start);
  if (block_num < 0) return block_num; // if blocknum < 0, there's an error, so return it

  // check permissions
  if (fat_permission(d_entry.file_attribute, mask) == 0) return 0;
  else return -EACCES;
}
