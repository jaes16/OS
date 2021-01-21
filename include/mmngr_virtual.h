#ifndef _MMNGR_VIRTUAL_H
#define _MMNGR_VIRTUAL_H

#include <vmmngr_pte.h>
#include <vmmngr_pde.h>
#include <system.h>
#include <stddef.h>

//! virtual address
typedef uint32_t virtual_addr;

//! i86 architecture defines 1024 entries per table--do not change
#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR	1024
#define PAGE_TABLE_SIZE 4096
#define PAGE_DIRECTORY_SIZE 4096

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)


//! page table
typedef struct ptable {
	pt_entry m_entries[PAGES_PER_TABLE];
}ptable;

//! page directory
typedef struct pdirectory {
	pd_entry m_entries[PAGES_PER_DIR];
}pdirectory;

extern void vmmngr_enable_paging (void);

//! allocates a page in physical memory
extern bool vmmngr_alloc_page (pt_entry* e);

//! frees a page in physical memory
extern void vmmngr_free_page (pt_entry* e);

//! switch to a new page directory
extern bool vmmngr_switch_pdirectory (pdirectory* pd);

//! get current page directory
extern pdirectory* vmmngr_get_directory (void);

//! flushes a cached translation lookaside buffer (TLB) entry
extern void vmmngr_flush_tlb_entry (virtual_addr addr);

//! clears a page table
extern void vmmngr_ptable_clear (ptable* p);

//! convert virtual address to page table index
extern uint32_t vmmngr_virt_to_ptable_index (virtual_addr addr);

//! get page entry from page table
extern pt_entry* vmmngr_ptable_lookup_entry (ptable* p,virtual_addr addr);

//! convert virtual address to page directory index
extern uint32_t vmmngr_pdirectory_virt_to_index (virtual_addr addr);

//! clears a page directory table
extern void vmmngr_pdirectory_clear (pdirectory* dir);

//! get directory entry from directory table
extern pd_entry* vmmngr_pdirectory_lookup_entry (pdirectory* p, virtual_addr addr);

//! initialize the memory manager
extern void vmmngr_initialize (void);

#endif
