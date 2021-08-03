#include <system.h>
#include <mmngr_phys.h>
#include <stdint.h>
#include <stddef.h>
#include <libc/string.h>



// size of physical memory
static	uint32_t	_mmngr_memory_size=0;

// number of blocks currently in use
static	uint32_t	_mmngr_used_blocks=0;

// maximum number of available memory blocks
static	uint32_t	_mmngr_max_blocks=0;

// memory map bit array. Each bit represents a memory block
static	uint32_t*	_mmngr_memory_map= 0;



// set to 1, if not already set, increase used blocks count
void mmap_set(int bit)
{
  if((_mmngr_memory_map[bit/PMMNGR_BLOCKS_PER_ENTRY] & (1 << (bit % PMMNGR_BLOCKS_PER_ENTRY))) == 0) _mmngr_used_blocks++;
  _mmngr_memory_map[bit / PMMNGR_BLOCKS_PER_ENTRY] |= (1 << (bit % PMMNGR_BLOCKS_PER_ENTRY));
}

void mmap_unset(int bit)
{
  if((_mmngr_memory_map[bit/PMMNGR_BLOCKS_PER_ENTRY] & (1 << (bit % PMMNGR_BLOCKS_PER_ENTRY))) != 0) _mmngr_used_blocks--;
  _mmngr_memory_map[bit / PMMNGR_BLOCKS_PER_ENTRY] &= ~ (1 << (bit % PMMNGR_BLOCKS_PER_ENTRY));
}

int mmap_test(int bit)
{
 return _mmngr_memory_map[bit / PMMNGR_BLOCKS_PER_ENTRY] &  (1 << (bit % PMMNGR_BLOCKS_PER_ENTRY));
}

int mmap_find_free(void)
{
	// find the first free bit
	for (int i=0; i< pmmngr_get_block_count() / PMMNGR_BLOCKS_PER_ENTRY; i++){
		if (_mmngr_memory_map[i] != 0xffffffff){
      int bit = 1;
			for (int j = 0; j < PMMNGR_BLOCKS_PER_ENTRY; j++) {		// test each bit in the dword
        if (!(_mmngr_memory_map[i] & bit)) return (i * PMMNGR_BLOCKS_PER_ENTRY) + j;
        bit = bit << 1;
			}
    }
  }
	return -1;
}

int mmap_find_free_s(int num_blocks)
{
  if(num_blocks == 0) return -1;

  if(num_blocks == 1) return mmap_find_free();

  // find the first free bit
  int ret_val;
  int num_current_blocks = 0;
	for (int i = 0; i < pmmngr_get_block_count() / PMMNGR_BLOCKS_PER_ENTRY; i++){
		if (_mmngr_memory_map[i] != 0xffffffff){
      int bit = 1;
			for (int j = 0; j < PMMNGR_BLOCKS_PER_ENTRY; j++) {		// test each bit in the dword
        if (!(_mmngr_memory_map[i] & bit)){
          if(num_current_blocks == 0) ret_val = (i * PMMNGR_BLOCKS_PER_ENTRY) + j;
          num_current_blocks++;
          if(num_current_blocks == num_blocks) return ret_val;
        }
        else num_current_blocks = 0;
        bit = bit << 1;
			}
    }
    else num_current_blocks = 0;
  }
	return -1;
}


int pmmngr_memory_size_kb(void){
  return _mmngr_memory_size;
}

int pmmngr_get_block_count(void){
  return _mmngr_max_blocks;
}

int pmmngr_get_free_block_count(void){
  return _mmngr_max_blocks - _mmngr_used_blocks;
}

// memSize is in kilobytes
void pmmngr_init(size_t memSize, int bitmap)
{
  _mmngr_memory_size	=	memSize;
	_mmngr_memory_map	=	(uint32_t*) bitmap;
	_mmngr_max_blocks	=	(pmmngr_memory_size_kb() / PMMNGR_BLOCK_SIZE) * 1024; // because memory size is in kb
	_mmngr_used_blocks	=	pmmngr_get_block_count();

  // By default, all of memory is in use. set each bit in each byte of the memory map to 1
	memset (_mmngr_memory_map, 0xff, pmmngr_get_block_count() / PMMNGR_BLOCKS_PER_BYTE);
}

// initializes the region in blocks, setting all the appropriate bits in the memory map to 0
void pmmngr_init_region(physical_addr base, size_t size)
{
  // calculate the bits to unset
  int base_block = base/PMMNGR_BLOCK_SIZE;
  int last_block = (base+size)/PMMNGR_BLOCK_SIZE;

  // unset those bits
  for(int i = base_block; i <= last_block; i++){
    mmap_unset(i);
  }

  mmap_set(0);
}

// deinitializes the region in blocks, setting all the appropriate bits in the memory map to 1
void pmmngr_deinit_region(physical_addr base, size_t size)
{
  // calculate the bits to set
  int base_block = base/PMMNGR_BLOCK_SIZE;
  int last_block = (base+size)/PMMNGR_BLOCK_SIZE;

  // set those bits
  for(int i = base_block; i <= last_block; i++){
    mmap_set(i);
  }
}

// deinitializes a block and returns the location of the block in physical memory
// returns -1 if there aren't any free blocks
void * pmmngr_alloc_block(void)
{
  // check if there are any free blocks
  if (pmmngr_get_free_block_count() <= 0) return (void *)-1; // if not return -1

  // get first free block
  int addr = mmap_find_free();
  mmap_set(addr);
  return ((void *) (addr * PMMNGR_BLOCK_SIZE));
}

// allocates a sequential number of blocks
void * pmmngr_alloc_blocks(int num_blocks)
{
  if (pmmngr_get_free_block_count() < num_blocks) return (void *)-1;
  int addr = mmap_find_free_s(num_blocks);
  if (addr != -1){
    pmmngr_deinit_region(addr * PMMNGR_BLOCK_SIZE, num_blocks * PMMNGR_BLOCK_SIZE);
    return ((void *) (addr * PMMNGR_BLOCK_SIZE));
  }
  else return (void *)-1;
}

// frees a block
void pmmngr_free_block(void *p)
{
  if((((int)p) % PMMNGR_BLOCK_SIZE) != 0) mmap_unset((((int)p) / PMMNGR_BLOCK_SIZE)+1);
  else mmap_unset(((int)p) / PMMNGR_BLOCK_SIZE);

  mmap_set(0);
}

void pmmngr_free_blocks(void *p, int num_blocks)
{
  pmmngr_init_region((int) p, num_blocks * PMMNGR_BLOCK_SIZE);
}
