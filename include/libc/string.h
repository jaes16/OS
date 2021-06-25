#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

extern void *memcpy(void *dest, const void *src, size_t count);
extern void *memset(void *dest, char val, size_t count);
extern void *memmove(void *dest, void *src, size_t count);
extern void *memcpyw(void *dest, const void *src, size_t count);
extern void *memsetd(void *dest, unsigned int val, size_t count);
extern void *memcpyd(void *dest, unsigned int val, size_t count);
extern unsigned short *memsetw(void *dest, unsigned short val, size_t count);
extern char *strstr(const char *haystack, const char *needle);
extern char *strnstr(const char *haystack, const char *needle, size_t len);
extern size_t strlen(const char *str);
extern size_t strnlen(const char *str, size_t maxlen);
extern int strcmp(const char *str1, const char *str2);
extern int strncmp(const char *str1, const char *str2, size_t n);
extern char *index(const char *str, char c);
extern char *indexn(const char *str, char c, size_t n);
extern char *rindex(const char *str, char c);
extern char *rindexn(const char *str, char c, size_t n);



#endif
