#include <mmngr_virtual.h>
#include <system.h>
#include <libc/string.h>

void vmmngr_enable_paging (void)
{
  __asm__ __volatile ("mov %cr0, %eax \n\tor $0x80000000, %eax \n\tmov %eax, %cr0");
}

// allocates a page in physical memory
bool vmmngr_alloc_page (pt_entry *e)
{
  // check if there is a free 4096 byte block in physical memory
  void* p = pmmngr_alloc_block();
	if (!p)	return false; // if not, return false

	// map it to the page
	pt_entry_set_frame(e, (physical_addr)p);
	pt_entry_add_attribute(e, I86_PTE_PRESENT);
  return true;
}

//! frees a page in physical memory
void vmmngr_free_page (pt_entry *e)
{
  pmmngr_free_block((void *) (((uint32_t) e) >> 12));
}

//! switch to a new page directory
bool vmmngr_switch_pdirectory (pdirectory *pd)
{
  if (!pd) return false;
  int placeholder = 0;
  // move address of new page directory into eax register then that into cr3
  __asm__ __volatile__ ("mov %1, %%eax \n\tmov %%eax, %%cr3" : "=r" (placeholder) : "r" (pd));
  return true;
}

//! get current page directory
pdirectory* vmmngr_get_directory (void)
{
  pdirectory *pd;
  __asm__ __volatile__ ("mov %%cr3, %0" : "=r" (pd));
  return pd;
}

// flushes a cached translation lookaside buffer (TLB) entry
void vmmngr_flush_tlb_entry (virtual_addr addr)
{
  int placeholder = 0;
  __asm__ __volatile__ ("push %%eax \n\tcli \n\tmov %1, %%eax \n\tinvlpg (%%eax) \n\tsti \n\tpop %%eax": "=r" (placeholder) : "r" (addr));
}

// clears a page table
void vmmngr_ptable_clear (ptable* pt)
{
  if(!pt) return;
  memset(pt, 0, PAGE_TABLE_SIZE);
}

// convert virtual address to page table index
uint32_t vmmngr_virt_to_ptable_index (virtual_addr addr)
{
  return ((addr >> 12) & 0x3ff);
}

// get page entry from page table
pt_entry* vmmngr_ptable_lookup_entry (ptable* pt,virtual_addr addr)
{
  if(!pt) return 0;

  return &(pt->m_entries[vmmngr_virt_to_ptable_index(addr)]);
}

// convert virtual address to page directory index
uint32_t vmmngr_virt_to_pdirectory_index (virtual_addr addr)
{
  return (addr >> 22);
}

// clears a page directory table
void vmmngr_pdirectory_clear (pdirectory* dir)
{
  if(!dir) return;

  memset(dir, 0, PAGE_DIRECTORY_SIZE);
}

// get directory entry from directory table
pd_entry* vmmngr_pdirectory_lookup_entry (pdirectory* pd, virtual_addr addr)
{
  if(!pd) return 0;

  // return the pde at the index of the corresponding virtual address
  return &(pd->m_entries[vmmngr_virt_to_pdirectory_index(addr)]);
}

//! initialize the memory manager
void vmmngr_initialize (void)
{

  /*
  // identity map the first 4mb (first page table)
  ptable *pde_1 = (ptable *) pmmngr_alloc_block();
  if(!pde_1) return;
  memset(pde_1, 0, PAGE_TABLE_SIZE);

  // pages are present and writable but not user accessible
  uint32_t attributes = 0;
  attributes |= I86_PTE_PRESENT | I86_PTE_WRITABLE;
  // physical address starts at 0 and increments 4096 bytes each time
  physical_addr p_addr = 0;

  // fill out identity mapped page table
  for(int i = 0; i < PAGES_PER_TABLE; i++){
    pt_entry_set_frame(&(pde_1->m_entries[i]), p_addr);
    pt_entry_add_attribute(&(pde_1->m_entries[i]), attributes);
    // each page's physical address is increased each time
    p_addr += PMMNGR_BLOCK_SIZE;
  }
  */

  ptable *pde_1 = (ptable *) pmmngr_alloc_block();
  if(!pde_1) return;
  memset(pde_1, 0, PAGE_TABLE_SIZE);

  uint32_t attributes = 0;
  attributes |= I86_PTE_PRESENT | I86_PTE_WRITABLE;
  physical_addr p_addr = 0;

  for(int i = 0; i < PAGES_PER_TABLE; i++){
    pt_entry_set_frame(&(pde_1->m_entries[i]), p_addr);
    pt_entry_add_attribute(&(pde_1->m_entries[i]), attributes);
    // each page's physical address is increased each time
    p_addr += PMMNGR_BLOCK_SIZE;
  }

  // map 3gb to 1mb, because the kernel is at 3gb virtual, but 1mb physical
  ptable *pde_768 = (ptable *) pmmngr_alloc_block();
  if(!pde_768) return;
  memset(pde_768, 0, PAGE_TABLE_SIZE);

  // first block starts at 1
  p_addr = 0x100000;

  // fill out virt 3gb
  for(int i = 0; i < PAGES_PER_TABLE; i++){
    pt_entry_set_frame(&(pde_768->m_entries[i]), p_addr);
    pt_entry_add_attribute(&(pde_768->m_entries[i]), attributes);
    // each page's physical address is increased each time
    p_addr += PMMNGR_BLOCK_SIZE;
  }

  // add these two page table pointers as pde in page diretory
  pdirectory *pd = (pdirectory *) pmmngr_alloc_block();
  if(!pd) return;
  memset(pd, 0, PAGE_DIRECTORY_SIZE);

  pd_entry_set_frame(&(pd->m_entries[0]), (physical_addr) pde_1);
  pt_entry_add_attribute(&(pd->m_entries[0]), attributes);
  pd_entry_set_frame(&(pd->m_entries[768]), (physical_addr) pde_768);
  pt_entry_add_attribute(&(pd->m_entries[768]), attributes);

  vmmngr_switch_pdirectory(pd);
  vmmngr_enable_paging();
}
