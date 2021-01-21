#ifndef _VMMNGR_PDE_H
#define _VMMNGR_PDE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <mmngr_phys.h>


typedef uint32_t pd_entry;

#define PDE_FRAME_SHIFT 12

enum PAGE_PDE_FLAGS {
	I86_PDE_PRESENT			=	1,			//0000000000000000000000000000001
	I86_PDE_WRITABLE		=	2,			//0000000000000000000000000000010
	I86_PDE_USER			=	4,			//0000000000000000000000000000100
	I86_PDE_PWT				=	8,			//0000000000000000000000000001000
	I86_PDE_PCD				=	0x10,		//0000000000000000000000000010000
	I86_PDE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PDE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PDE_4MB				=	0x80,		//0000000000000000000000010000000
	I86_PDE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PDE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
  I86_PDE_ATTRIBUTES = 0x00000FFF,
  I86_PDE_FRAME			=	0xFFFFF000 	//1111111111111111111000000000000
};


extern void pd_entry_set_frame(pd_entry *pde, physical_addr addr);
extern void pd_entry_add_attribute(pd_entry *pde, uint32_t attribute);
extern void pd_entry_del_attribute(pd_entry *pde, uint32_t attribute);
extern int pd_entry_is_present(pd_entry pde);
extern bool pd_entry_is_user (pd_entry pde);
extern bool pd_entry_is_4mb (pd_entry pde);
extern int pd_entry_is_writable(pd_entry pde);
extern physical_addr pd_entry_frame(pd_entry pde);

#endif
