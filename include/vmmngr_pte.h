#ifndef _VMMNGR_PTE_H
#define _VMMNGR_PTE_H

#include <stdint.h>
#include <stddef.h>
#include <mmngr_phys.h>

typedef uint32_t pt_entry;

#define PTE_FRAME_SHIFT 12

enum pte_PTE_FLAGS { // i86 arch standard
	I86_PTE_PRESENT			=	1,			//0000000000000000000000000000001
	I86_PTE_WRITABLE		=	2,			//0000000000000000000000000000010
	I86_PTE_USER			=	4,			//0000000000000000000000000000100
	I86_PTE_WRITETHOUGH		=	8,			//0000000000000000000000000001000
	I86_PTE_NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
	I86_PTE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PTE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PTE_PAT				=	0x80,		//0000000000000000000000010000000
	I86_PTE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PTE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
  I86_PTE_ATTRIBUTES = 0x00000FFF,
  I86_PTE_FRAME			=	0xFFFFF000    //11111111111111111111000000000000
};

extern void pt_entry_set_frame(pt_entry *pte, physical_addr addr);
extern void pt_entry_add_attribute(pt_entry *pte, uint32_t attribute);
extern void pt_entry_del_attribute(pt_entry *pte, uint32_t attribute);
extern void pt_entry_enable_global (pt_entry *pte);
extern int pt_entry_is_present(pt_entry pte);
extern int pt_entry_is_writable(pt_entry pte);
extern physical_addr pt_entry_frame(pt_entry pte);

#endif
