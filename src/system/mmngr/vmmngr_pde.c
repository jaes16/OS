#include <stdint.h>
#include <vmmngr_pde.h>


typedef uint32_t pd_entry;

inline void pd_entry_set_frame(pd_entry *pde, physical_addr addr)
{
  // remove old frame, which are the top 20 bits, the bottom 12 bits saved for attributes
  *pde &= I86_PDE_ATTRIBUTES;
  // set new frame
  *pde |= (uint32_t) (addr & I86_PDE_FRAME);
}

inline void pd_entry_add_attribute(pd_entry *pde, uint32_t attribute)
{
  // add attribute
  *pde |= attribute;
}

inline void pd_entry_del_attribute(pd_entry *pde, uint32_t attribute)
{
  // remove attribute
  *pde &= ~attribute;
}


inline int pd_entry_is_present(pd_entry pde)
{
  // return 0st bit
  return pde & I86_PDE_PRESENT;
}

//! test if directory is user mode
inline bool pd_entry_is_user (pd_entry pde)
{
  // return 2rd bit
  return pde & I86_PDE_USER;
}

//! test if directory contains 4mb pages
inline bool pd_entry_is_4mb (pd_entry pde)
{
  // return 7th bit
  return pde & I86_PDE_4MB;
}

inline int pd_entry_is_writable(pd_entry pde)
{
  // return 2nd bit
  return pde & I86_PDE_WRITABLE;
}

inline physical_addr pd_entry_frame(pd_entry pde)
{
  // return physical address of the page table
  return pde & I86_PDE_FRAME;
}
