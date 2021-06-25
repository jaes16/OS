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



#define TERM_MAX_COM_LEN 80



char *command_calls[] = {
	"echo", "time", "readdisk", "readdisk-hex", "writedisk", "ls", "cd", "mkdir", "rmdir", "shutdown", "help"
};

int command_lens[] = {4, 4, 8, 12, 9, 2, 2, 5, 5, 8, 4};

int num_commands = 11;


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



void terminal_echo(char *command){
	(void)(command);
	printf("Echo is a figure from Greek mythology.\n");
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
	int call_len = 2;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);

	char path[FAT_MAX_PATH_LEN];
	memset(path, 0, FAT_MAX_PATH_LEN);

	if(len_command < (call_len+2) || command[call_len] != ' ' || command[call_len+1] == ' '){
		// call readdir on current path
		memcpy(path, fat_cur_path, FAT_MAX_PATH_LEN);
	}
	else{
		// until next ' '
		for(int i = call_len+1; i < len_command; i++){
			if(command[i] == ' ') break;
			path[i-(call_len+1)] = command[i];
		}

	}


	fat_dir_entry buf[fat_ents_per_blck()+1];
	memset(buf, 0, FAT_BLOCK_SIZE);
	int offset = 0;
	offset = fat_readdir(path, buf, FAT_BLOCK_SIZE, (size_t) offset);
	if(offset < 0) terminal_throw_error(offset);

	// read all directory entries until they stop returning anything
	while(buf[0].file_name[0] != 0){
		// print all the file names
		for(uint32_t i = 0; i < fat_ents_per_blck(); i++){
			if (buf[i].file_name[0] == 0) break;
			print_permissions(buf[i].file_attribute);
			printf("\t%d\t%s\n", buf[i].file_size, buf[i].file_name);
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
	int call_len = 2;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);

	char path[FAT_MAX_PATH_LEN];
	memset(path, 0, FAT_MAX_PATH_LEN);

	// get path
	if(len_command < (call_len+2) || command[call_len] != ' ' || command[call_len+1] == ' '){
		// call readdir on current path
		memcpy(path, fat_cur_path, FAT_MAX_PATH_LEN);
	}
	else{
		// until next ' '
		for(int i = call_len+1; i < len_command; i++){
			if(command[i] == ' ') break;
			path[i-(call_len+1)] = command[i];
		}
	}
	int cur_path_len = strnlen(fat_cur_path, FAT_MAX_PATH_LEN);

	// check path len
	if (path[0] != '/'){
		if ((strnlen(path,FAT_MAX_PATH_LEN) + cur_path_len + 1) > FAT_MAX_PATH_LEN) terminal_throw_error(-ENAMETOOLONG);
	}
	// see if we can enter this file
	int res = fat_access(path, FAT_PERM_OWN_EXEC);
	if (res < 0){
		terminal_throw_error(res);
		return;
	}

	// get file_attributes
	fat_dir_entry d_entry;

	if (path[0] != '/'){
		fat_cur_ent_block = fat_find_entry(path, &d_entry, fat_cur_entry);
		memcpy(fat_cur_entry, &d_entry, sizeof(fat_dir_entry));
		if(fat_cur_path[cur_path_len-1] == '/'){
			memcpy(fat_cur_path+cur_path_len, path, strnlen(path,FAT_MAX_PATH_LEN));
		}
		else{
			fat_cur_path[cur_path_len] = '/';
			memcpy(fat_cur_path+cur_path_len+1, path, strnlen(path,FAT_MAX_PATH_LEN));
		}
	}
	else{
		// update current entry and current path
		fat_cur_ent_block = fat_find_entry(path, &d_entry, fat_root_entry);
		memcpy(fat_cur_entry, &d_entry, sizeof(fat_dir_entry));
		memset(fat_cur_path, 0, FAT_MAX_PATH_LEN);
		memcpy(fat_cur_path, path, strnlen(path,FAT_MAX_PATH_LEN));
	}

	path_remove_dots(fat_cur_path);

	return;

}


void terminal_mkdir(char *command)
{
	// not supporting flags yet
	int call_len = 5;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);

	char path[FAT_MAX_PATH_LEN];
	memset(path, 0, FAT_MAX_PATH_LEN);

	if(len_command < (call_len+2) || command[call_len] != ' ' || command[call_len+1] == ' '){
		// call readdir on current path
		memcpy(path, fat_cur_path, FAT_MAX_PATH_LEN);
	}
	else{
		// until next ' '
		for(int i = call_len+1; i < len_command; i++){
			if(command[i] == ' ') break;
			path[i-(call_len+1)] = command[i];
		}
	}

	int res = fat_mkdir(path, FAT_PERM_OWN_READ | FAT_PERM_OWN_WRITE | FAT_PERM_OWN_EXEC |
	FAT_PERM_GRP_READ | FAT_PERM_GRP_EXEC | FAT_PERM_OTH_READ | FAT_PERM_OTH_EXEC);
	if(res < 0) terminal_throw_error(res);
	return;
}


void terminal_rmdir(char *command)
{
	// not supporting flags yet
	int call_len = 5;
	int len_command = strnlen(command, TERM_MAX_COM_LEN);

	char path[FAT_MAX_PATH_LEN];
	memset(path, 0, FAT_MAX_PATH_LEN);

	if(len_command < (call_len+2) || command[call_len] != ' ' || command[call_len+1] == ' '){
		// call readdir on current path
		memcpy(path, fat_cur_path, FAT_MAX_PATH_LEN);
	}
	else{
		// until next ' '
		for(int i = call_len+1; i < len_command; i++){
			if(command[i] == ' ') break;
			path[i-(call_len+1)] = command[i];
		}
	}

	int res = fat_rmdir(path);
	if(res < 0) terminal_throw_error(res);
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
	terminal_ls, terminal_cd, terminal_mkdir, terminal_rmdir, terminal_shutdown, terminal_help
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
