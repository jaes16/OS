#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

extern void *memcpy(void *dest, const void *src, size_t count);
extern void *memset(void *dest, char val, size_t count);
extern void *memcpyw(void *dest, const void *src, size_t count);
extern unsigned short *memsetw(void *dest, unsigned short val, size_t count);
extern int strlen(const char *str);
extern int strcmp(const char *str1, const char *str2);

#endif
