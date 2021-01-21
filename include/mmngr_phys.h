#ifndef _MMNGR_PHYS_H
#define _MMNGR_PHYS_H

#include <stdint.h>
#include <stddef.h>

//! 8 blocks per byte
#define PMMNGR_BLOCKS_PER_BYTE 8

#define PMMNGR_BLOCKS_PER_ENTRY 32

//! block size (4k)
#define PMMNGR_BLOCK_SIZE	4096

//! block alignment
#define PMMNGR_BLOCK_ALIGN	4096

typedef	uint32_t physical_addr;



extern void mmap_set(int bit);
extern void mmap_unset(int bit);
extern int mmap_test(int bit);
extern int mmap_find_free(void);

extern void pmmngr_init(size_t memSize, int bitmap);
extern int pmmngr_get_memory_size(void);
extern int pmmngr_get_block_count(void);
extern int pmmngr_get_free_block_count(void);
extern void pmmngr_init_region(physical_addr base, size_t size);
extern void pmmngr_deinit_region(physical_addr base, size_t size);
extern void * pmmngr_alloc_block(void);
extern void * pmmngr_alloc_blocks(int num_block);
extern void pmmngr_free_block(void *p);
extern void pmmngr_free_blocks(void *p, int num_blocks);

#endif
