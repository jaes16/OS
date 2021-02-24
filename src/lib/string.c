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

int strlen(const char *str)
{
  int i = 0;
  while(str[i] != 0){
    i++;
  }
  return i;
}

int strcmp(const char *str1, const char *str2)
{
  int strlen1 = strlen(str1);

  if(strlen1 > strlen(str2)) return 1;
  else if(strlen1 < strlen(str2)) return -1;

  // if they're the same size check characters
  for(int i = 0; i < strlen(str1); i++){
    if(str1[i] == str2[i]);
    else if(str1[i] > str2[i]) return 1;
    else return -1;
  }

  return 0;
}
