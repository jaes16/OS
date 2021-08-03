#include <terminal.h>

#include <system.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>

#include <libc/string.h>
#include <libc/stdio.h>

#include <multiboot.h>

#include <mmngr_virtual.h>

#include <ata.h>
#include <fat_fs.h>







char *command_calls[] = {
	"echo", "time", "readdisk", "readdisk-hex", "writedisk", "ls", "cd", "mkdir",
	"rmdir", "mknod", "rm", "less", "hexdump", "shutdown", "help"
};

int command_lens[] = {4, 4, 8, 12, 9, 2, 2, 5, 5, 5, 2, 4, 7, 8, 4};

int num_commands = 15;


void terminal_throw_error(int error)
{
	switch (error){
		case -EPERM: // 1
		printf("Operation not permitted.\n");
		break;
		case -ENOENT: // 2
		printf("No such file or directory.\n");
		break;
		case -EINTR: // 4
		printf("Interrupted system call.\n");
		break;
		case -EIO: // 5
		printf("Input/Output error.\n");
		break;
		case -ENXIO: // 6
		printf("Device not configured.\n");
		break;
		case -ENOMEM: // 12
		printf("No available memory.\n");
		break;
		case -EACCES: // 13
		printf("Permission denied.\n");
		break;
		case -EFAULT: // 14
		printf("Bad address.\n");
		break;
		case -EEXIST: // 17
		printf("File or directory already exists.\n");
		break;
		case -ENOTDIR: // 20
		printf("Is not a directory.\n");
		break;
		case -EISDIR: // 21
		printf("Is a directory.\n");
		break;
		case -ENOSPC: // 28
		printf("No space on device.\n");
		break;
		case -ELOOP: // 62
		printf("Too many symbolic links.\n");
		break;
		case -ENAMETOOLONG: // 63
		printf("File/Path name is too long.\n");
		break;
		case -ENOTEMPTY: // 66
		printf("Directory is not empty.\n");
		break;
		case -ENOSYS: // 78
		printf("Function not implemented.\n");
		break;
	}
}



void path_remove_dots(char *path){
	int path_len = strnlen(path, FAT_MAX_PATH_LEN);

	char *d_index = strnstr(path, "/./", path_len);
	while(d_index != NULL){
		int len = (path_len - (d_index-path)) - 2;
		memmove(d_index, d_index + 2, len);
		path_len = path_len - 2;
		path[path_len] = 0;
		d_index = strnstr(path, "/./", path_len);
	}

	d_index = strnstr(path, "/../", path_len);
	while(d_index != NULL){
		int len = (path_len - (d_index-path)) - 2;
		memmove(d_index, d_index + 3, len);
		path_len -= 3;
		if(d_index != path){
			char *temp = rindexn(path, '/', d_index-(path+1));
			memmove(temp, d_index, (path_len - (d_index-path)));
			path_len -= (d_index-temp);
			path[path_len] = 0;
		}
		d_index = strnstr(path, "/../", path_len);
	}

	memset(path+path_len, 0, FAT_MAX_PATH_LEN-path_len);

	if(strcmp(path, "/..") == 0){
		// at beginning
		path[1] = 0;
		path[2] = 0;
		return;
	}

	if(strcmp(path+path_len-3, "/..") == 0){
		// at end
		path[path_len-3] = 0;
		path[path_len-2] = 0;
		path[path_len-1] = 0;
		path_len -= 3;
		d_index = rindex(path, '/');
		if (d_index == path){
			memset(path, 0, FAT_MAX_PATH_LEN);
			path[0] = '/';
			return;
		}
		if (d_index != NULL){
			path_len = d_index - path;
			memset(path+path_len, 0, FAT_MAX_PATH_LEN-path_len);
		}
	}

	// if at beginning without anything else
	if(strcmp(path, "/.") == 0){
		path[1] = 0;
		return;
	}
	// if at end
	if(strcmp(path+path_len-2, "/.") == 0){
		path[path_len-1] = 0;
		path[path_len-2] = 0;
		return;
	}
}



void correct_path(char *path){
	int path_len = strnlen(path, FAT_MAX_PATH_LEN);

	// remove excess '/'s
	for(int i = 0; i < path_len-1;){
		if(path[i] == '/' && path[i+1] == '/') {
			memmove(path+i, path+i+1, path_len-(i+1));
			path_len--;
			path[path_len] = 0;
		}
		else i++;
	}
	if (path_len > 1 && path[path_len-1] == '/') path[path_len-1] = 0;

	path_remove_dots(path);
	return;
}



int get_target(int call_len, char *command, char *path)
{
	int len_command = strnlen(command, TERM_MAX_COM_LEN);

	memset(path, 0, FAT_MAX_PATH_LEN);

	if(len_command < (call_len+2) || command[call_len] != ' ' || command[call_len+1] == ' '){
		// no target
		return -1;
	}
	else{
		// until next ' '
		for(int i = call_len+1; i < len_command; i++){
			if(command[i] == ' ') break;
			path[i-(call_len+1)] = command[i];
		}
	}

	return len_command;
}




void terminal_echo(char *command)
{
	// not supporting flags yet
	int call_len = 4;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);

	char path[FAT_MAX_PATH_LEN];
	memset(path, 0, FAT_MAX_PATH_LEN);

	int i = call_len+1;
	// get path
	if(len_command < (call_len+2) || command[call_len] != ' ' || command[call_len+1] == ' '){
		printf("\n");
		return;
	}
	else{
		// until next >
		for(; i < len_command; i++){
			if(command[i] == '>') break;
		}
		for(int j = i+2; j < len_command; j++){
			path[j-(i+2)] = command[j];
		}
	}
	if (i == len_command) printf("%s\n", (char *) command+call_len+1);
	else {
		size_t write_size = i - (call_len+1);

		// gotta write to file
		char buf[FAT_BLOCK_SIZE];
		memset(buf, 0, FAT_BLOCK_SIZE);
		memcpy(buf, command+call_len+1, write_size-1);
		buf[write_size-1] = '\n';

		correct_path(path);

		// set file size to zero then increase to needed amount
		int res = fat_truncate(path, 0);
		if (res < 0){ terminal_throw_error(res); return;}
		res = fat_truncate(path, write_size);
		if (res < 0){ terminal_throw_error(res); return;}

		// writeout
		res = fat_write(path, buf, write_size, 0);
		if (res < 0) terminal_throw_error(res);
		// we know command length will not be > FAT_BLOCK_SIZE
	}
	return;
}




void terminal_time(char *command){
	(void)(command);
	printf("Time: %d\n", timer_ticks);
}




void terminal_readdisk(char *command){
	int call_len = 8;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);
	if(len_command < (call_len+2) || command[call_len] != ' ') printf("Invalid command format for readdisk.\n");
	else{
		int sector = 0;
		for(int i = call_len+1; i < len_command; i++){
			if(command[i] < '0' || command[i] > '9'){
				printf("Invalid command format for readdisk.\n");
				return;
			}
			sector = sector * 10;
			sector = sector + ((int) command[i] - '0');
		}

		printf("Reading in sector %x from disk...\n", sector);
		char buf[512];
		memset(buf, 0, 512);
		ATA_read_sectors(buf, sector, 1);
		printf("Sector %x:\n%s\n\n", sector, buf);
	}
}

void terminal_readdisk_hex(char *command){
	int call_len = 12;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);
	if(len_command < (call_len+2) || command[call_len] != ' ') printf("Invalid command format for readdisk_hex.\n");
	else{
		int sector = 0;
		for(int i = (call_len+1); i < len_command; i++){
			if(command[i] < '0' || command[i] > '9'){
				printf("Invalid command format for readdisk_hex.\n");
				return;
			}
			sector = sector * 10;
			sector = sector + ((int) command[i] - '0');
		}

		printf("Reading in sector %x from disk...\n", sector);
		int buf[128];
		memset(buf, 0, 512);
		ATA_read_sectors(buf, sector, 1);
		printf("Sector %x:\n", sector);
		for(int i = 0; i < 128; i++) printf("%x\t", buf[i]);
		printf("\n");
	}
}



void terminal_writedisk(char *command){
	int call_len = 9;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);
	if(len_command < (call_len+2) || command[call_len] != ' ') printf("Invalid command format for writedisk.\n");
	else{
		int sector = 0;
		int i = 0;
		for(i = (call_len+1); i < len_command; i++){
			if(command[i] < '0' || command[i] > '9'){
				if(command[i] == ' ') break;
				else{
					printf("Invalid command format for writedisk.\n");
					return;
				}
			}
			sector = sector * 10;
			sector = sector + ((int) command[i] - '0');
		}
		if (i == len_command){
			printf("Invalid command format for writedisk.\n");
			return;
		}
		i++;
		printf("Writing to sector %x in disk...\n", sector);
		char buf[512];
		memset(buf, 0, 512);
		memcpy(buf, command+i, len_command-i);
		ATA_write_sectors(buf, sector, 1);
	}
}


void print_permissions(uint32_t perm)
{
	if((perm & FAT_FILE_ATTR_DIR) != 0) printf("d");
	else printf("-");
	if((perm & FAT_PERM_OWN_READ) != 0) printf("r");
	else printf("-");
	if((perm & FAT_PERM_OWN_WRITE) != 0) printf("w");
	else printf("-");
	if((perm & FAT_PERM_OWN_EXEC) != 0) printf("x");
	else printf("-");
	if((perm & FAT_PERM_GRP_READ) != 0) printf("r");
	else printf("-");
	if((perm & FAT_PERM_GRP_WRITE) != 0) printf("w");
	else printf("-");
	if((perm & FAT_PERM_GRP_EXEC) != 0) printf("x");
	else printf("-");
	if((perm & FAT_PERM_OTH_READ) != 0) printf("r");
	else printf("-");
	if((perm & FAT_PERM_OTH_WRITE) != 0) printf("w");
	else printf("-");
	if((perm & FAT_PERM_OTH_EXEC) != 0) printf("x");
	else printf("-");
}

void terminal_ls(char *command)
{
	// not supporting flags yet
	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(2, command, path);

	// if no target, run on current directory
	if(len_command < 0)	memcpy(path, fat_cur_path, FAT_MAX_PATH_LEN);

	fat_dir_entry buf[fat_ents_per_blck()+1];
	memset(buf, 0, FAT_BLOCK_SIZE);
	int offset = 0;

	correct_path(path);

	offset = fat_readdir(path, buf, FAT_BLOCK_SIZE, (size_t) offset);
	if(offset < 0) terminal_throw_error(offset);

	// read all directory entries until they stop returning anything
	while(buf[0].file_name[0] != 0){
		// print all the file names
		for(uint32_t i = 0; i < fat_ents_per_blck(); i++){
			if (buf[i].file_name[0] == 0) break;
			print_permissions(buf[i].file_attribute);
			printf("\t%d\t#%d\t%s\n", buf[i].file_size, buf[i].cluster_num, buf[i].file_name);
		}
		// read more
		memset(buf, 0, FAT_BLOCK_SIZE);
		offset = fat_readdir(path, buf, FAT_BLOCK_SIZE, (size_t) offset);
		if (offset < 0) terminal_throw_error(offset);
	}
}


void terminal_cd(char *command)
{
	// not supporting flags yet
	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(2, command, path);

	// if no target, try to change directory to root
	if (len_command < 0) path[0] = '/';

	int cur_path_len = strnlen(fat_cur_path, FAT_MAX_PATH_LEN);

	// get file_attributes
	fat_dir_entry d_entry;
	int res = fat_getattr(path, &d_entry);
	if (res < 0){ terminal_throw_error(res); return;}


	int block_num = 0;
	if (path[0] != '/'){
		if ((strnlen(path,FAT_MAX_PATH_LEN) + cur_path_len + 1) > FAT_MAX_PATH_LEN){
			terminal_throw_error(-ENAMETOOLONG);
			return;
		}

		correct_path(path);

		// find directory from current directory
		block_num = fat_find_entry(path, &d_entry, fat_cur_entry);

		// check if directory
		if ((d_entry.file_attribute & FAT_FILE_ATTR_DIR) == 0) {terminal_throw_error(-ENOTDIR); return;}
		// check if it can be entered
		if ((d_entry.file_attribute & FAT_PERM_OWN_EXEC) == 0) {terminal_throw_error(-EACCES); return;}

		// set current path
		fat_cur_path[cur_path_len] = '/';
		memcpy(fat_cur_path+cur_path_len+1, path, strnlen(path,FAT_MAX_PATH_LEN));
	}
	else{

		correct_path(path);

		// update current entry and current path
		block_num = fat_find_entry(path, &d_entry, fat_root_entry);

		// check if directory
		if ((d_entry.file_attribute & FAT_FILE_ATTR_DIR) == 0) {terminal_throw_error(-ENOTDIR); return;}
		// check if it can be entered
		if ((d_entry.file_attribute & FAT_PERM_OWN_EXEC) == 0) {terminal_throw_error(-EACCES); return;}

		// set current path
		memset(fat_cur_path, 0, FAT_MAX_PATH_LEN);
		memcpy(fat_cur_path, path, strnlen(path,FAT_MAX_PATH_LEN));
	}

	// update current directory and the block that the dir entry for the current directory is in
	memcpy(fat_cur_entry, &d_entry, sizeof(fat_dir_entry));
	fat_cur_ent_block = block_num;

	// remove dots
	correct_path(fat_cur_path);

	return;

}


void terminal_mkdir(char *command)
{
	// not supporting flags yet
	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(5, command, path);


	if (len_command < 0) { printf("mkdir requires a target.\n"); return;}

	correct_path(path);

	int res = fat_mkdir(path, FAT_PERM_OWN_READ | FAT_PERM_OWN_WRITE | FAT_PERM_OWN_EXEC |
	FAT_PERM_GRP_READ | FAT_PERM_GRP_EXEC | FAT_PERM_OTH_READ | FAT_PERM_OTH_EXEC);
	if(res < 0) terminal_throw_error(res);
	return;
}


void terminal_rmdir(char *command)
{
	// not supporting flags yet
	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(5, command, path);

	if(len_command < 0){	printf("rmdir requires a target.\n"); return;}

	correct_path(path);

	int res = fat_rmdir(path);
	if(res < 0) terminal_throw_error(res);
	return;
}



void terminal_mknod(char *command)
{
	// not supporting flags yet
	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(5, command, path);

	if(len_command < 0){ printf("mknod requires a target.\n"); return;}

	correct_path(path);

	int res = fat_mknod(path, FAT_PERM_OWN_READ | FAT_PERM_OWN_WRITE | FAT_PERM_OWN_EXEC |
	FAT_PERM_GRP_READ | FAT_PERM_GRP_EXEC | FAT_PERM_OTH_READ | FAT_PERM_OTH_EXEC);
	if(res < 0) terminal_throw_error(res);
	return;
}



void terminal_rm(char *command)
{

	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(2, command, path);

	if(len_command < 0)	{printf("less requires a target.\n"); return;}

	correct_path(path);

	int res = fat_unlink(path);
	if(res < 0) terminal_throw_error(res);
	return;

}



void terminal_less(char *command)
{
	// not supporting flags yet
	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(4, command, path);

	if(len_command < 0){	printf("less requires a target.\n");	return;	}

	char buf[FAT_BLOCK_SIZE];
	memset(buf, 0, FAT_BLOCK_SIZE);
	size_t offset = 0;

	correct_path(path);

	//start reading
	int res = fat_read(path, buf, FAT_BLOCK_SIZE, (size_t) offset);
	if(res < 0) terminal_throw_error(res);

	// read all directory entries until they stop returning anything
	while(res > 0){
		// print
		printf("%s\n", buf);
		// read more
		memset(buf, 0, FAT_BLOCK_SIZE);
		offset += res;
		res = fat_read(path, buf, FAT_BLOCK_SIZE, (size_t) offset);
		if (res < 0) terminal_throw_error(offset);
	}
	return;
}



void terminal_hexdump(char *command)
{
	// not supporting flags yet
	char path[FAT_MAX_PATH_LEN];
	int len_command = get_target(7, command, path);

	if(len_command < 0) {	printf("hexdump requires a target.\n");	return;	}

	char buf[FAT_BLOCK_SIZE];
	memset(buf, 0, FAT_BLOCK_SIZE);
	size_t offset = 0;

	correct_path(path);

	// start reading
	int res = fat_read(path, buf, FAT_BLOCK_SIZE, (size_t) offset);
	if(res < 0) terminal_throw_error(res);

	// read all directory entries until they stop returning anything
	while(res > 0){
		// print in hex
		printf("%x\t", offset);
		for (int i = 0; i < FAT_BLOCK_SIZE/16; i++){
			for (int j = 0; j < 16; j++){
				printf("%x  ", ((int) buf[(16 * i) + j]) & 0xffff);
			}
			printf("\n");
		}
		// read more
		memset(buf, 0, FAT_BLOCK_SIZE);
		offset += res;
		res = fat_read(path, buf, FAT_BLOCK_SIZE, (size_t) offset);
		if (res < 0) terminal_throw_error(offset);
	}
	return;
}



void terminal_shutdown(char *command)
{
	(void)(command);
	printf("Shutting down...");
	outportw(0x604, 0x2000);
	return;
}


void terminal_help(char *command)
{
	(void)(command);
	printf("Supported commands:\n");
	for (int i = 0; i < num_commands; i++){
		printf("\t%s\n", command_calls[i]);
	}
}


void *commands[] = {
	terminal_echo, terminal_time, terminal_readdisk, terminal_readdisk_hex, terminal_writedisk,
	terminal_ls, terminal_cd, terminal_mkdir, terminal_rmdir, terminal_mknod, terminal_rm, terminal_less,
	terminal_hexdump, terminal_shutdown, terminal_help
};


int terminal_command(char *command)
{
	int strlen_command = strnlen(command, TERM_MAX_COM_LEN);
	int len_command = 0;
	for(int i = 0; i < strlen_command; i++){
		if (command[i] == ' ') break;
		len_command++;
	}
	if (len_command != 0){
		for(int i = 0; i < num_commands; i++){
			if(command_lens[i] == len_command){
				if (strncmp(command_calls[i], command, len_command) == 0){
					((void(*)(char *))commands[i])(command);
					return 1;
				}
			}
		}
	}

	printf("Undefined command: %s\n Use command \"help\" to get a list of supported commands.\n", command);
	return 1;
}
