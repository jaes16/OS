#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <libc/string.h>


// copies count bytes of src to dest, but if range src:src+count is within dest:dest+count, src may change before written
void *memcpy(void* dest, const void *src, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((char *)dest)[i] = ((char *)src)[i];
  }
  return dest;
}

// sets count bytes of dest to the value in val
void *memset(void* dest, char val, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((char *)dest)[i] = val;
  }
  return dest;
}

void *memmove(void *dest, void *src, size_t count)
{
	// check overlap
	if(dest >= src && dest < src+count){
		for(size_t i = count; i > 0; i--){
			((char *) dest)[i-1] = ((char *) src)[i-1];
		}
	}
	else{
		for(size_t i = 0; i < count; i++){
	    ((char *)dest)[i] = ((char *)src)[i];
	  }
	}

	return dest;
}

// copies count bytes of src to dest, but if range src:src+count is within dest:dest+count, src may change before written
void *memcpyw(void* dest, const void *src, size_t count)
{
  unsigned short temp[count];
  for(size_t i = 0; i < count; i++){
    temp[i] = ((unsigned short *)src)[i];
  }
  for(size_t i = 0; i < count; i++){
    ((unsigned short *)dest)[i] = temp[i];
  }
  return dest;
}

// sets count shortbytes of dest to the value in val
unsigned short *memsetw(void *dest, unsigned short val, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((unsigned short *)dest)[i] = val;
  }
  return dest;
}

void *memsetd(void *dest, unsigned int val, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((unsigned int *)dest)[i] = val;
  }
  return dest;
}

void *memcpyd(void *dest, unsigned int val, size_t count)
{
  for(size_t i = 0; i < count; i++){
    ((unsigned int *)dest)[i] = val;
  }
  return dest;
}

extern char *strstr(const char *haystack, const char *needle)
{
	size_t len_hay = strlen(haystack);
	size_t len_need = strlen(needle);
	if (len_hay == 0) return NULL;
	if (len_need == 0) return (char *) haystack;
	for(size_t i = 0; i < len_hay-len_need; i++){
		if(haystack[i] == needle[0]){
			if(strncmp(haystack+i, needle, len_need) == 0) return (char *) haystack+i;
		}
	}
	return NULL;
}

extern char *strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t len_hay = strnlen(haystack, len);
	size_t len_need = strlen(needle);
	if (len_hay == 0) return NULL;
	if (len_need == 0) return (char *) haystack;
	for(size_t i = 0; i < len_hay-len_need && i < len; i++){
		if(haystack[i] == needle[0]){
			if(strncmp(haystack+i, needle, len_need) == 0) return (char *) haystack+i;
		}
	}
	return NULL;
}

size_t strlen(const char *str)
{
	if(str == NULL) return 0;
  size_t i = 0;
  while(str[i] != 0){
    i++;
  }
  return i;
}

size_t strnlen(const char *str, size_t max_len)
{
  size_t i = 0;
  while((i < max_len) && (str[i] != 0)){
    i++;
  }
  return i;
}

int strcmp(const char *str1, const char *str2)
{
  // if they're the same size check characters
  int i = 0;
  while(str1[i] != 0){
    if(str1[i] == str2[i]);
    else if(str1[i] > str2[i]) return 1;
    else return -1;
    i++;
  }
  if (str2[i] != 0) return -1;
  return 0;
}

int strncmp(const char *str1, const char *str2, size_t n)
{
  // if they're the same size check characters
  size_t i = 0;
  while((i < n) && (str1[i] != 0)){
    if(str1[i] == str2[i]);
    else if(str1[i] > str2[i]) return 1;
    else return -1;
    i++;
  }
  if((i != n) && (str2[i] != 0)) return -1;
  return 0;
}

char *index(const char *str, char c)
{
  size_t i = 0;
  while(str[i] != 0){
    if (str[i] == c) return ((char *)str) + i;
    i++;
  }
  if (c == 0) return ((char *)str) + i;
  return NULL;
}

char *indexn(const char *str, char c, size_t n)
{
  size_t i = 0;
  while(str[i] != 0 && i < n){
    if (str[i] == c) return ((char *)str) + i;
    i++;
  }
  if (c == 0) return ((char *)str) + i;
  return NULL;
}

char *rindex(const char *str, char c)
{
	int i = strlen(str) - 1;
	if (c == 0) return ((char *)str) + i + 1;
	while(i >= 0){
		if (str[i] == c) return ((char *) str) + i;
		i--;
	}
	return NULL;
}

char *rindexn(const char *str, char c, size_t n)
{
	int i = strlen(str);
	if ((size_t) i > n) i = n;
	if (c == 0 && (size_t) i != n) return ((char *)str) + i + 1;
	i--;
	while(i >= 0){
		if (str[i] == c) return ((char *) str) + i;
		i--;
	}
	return NULL;
}
