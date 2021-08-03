#ifndef _TERMINAL_H
#define _TERMINAL_H

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
#include <pci.h>
#include <fat_fs.h>

#define TERM_MAX_COM_LEN 80

extern int terminal_command(char *command);



#endif
