#include <stdint.h>
#include <vmmngr_pte.h>


inline void pt_entry_set_frame(pt_entry *pte, physical_addr addr)
{
  // remove old frame, which are the top 20 bits, the bottom 12 bits saved for attributes
  *pte &= I86_PTE_ATTRIBUTES;
  // set new frame
  *pte |= (uint32_t) (addr & I86_PTE_FRAME);
}

inline void pt_entry_add_attribute(pt_entry *pte, uint32_t attribute)
{
  // add attribute
  *pte |= attribute;
}

inline void pt_entry_del_attribute(pt_entry *pte, uint32_t attribute)
{
  // remove attribute
  *pte &= ~attribute;
}

inline void pt_entry_enable_global (pt_entry *pte)
{
  // return 8th bit
  *pte |= I86_PTE_CPU_GLOBAL;
}

inline int pt_entry_is_present(pt_entry pte)
{
  // return zeroth bit
  return pte & I86_PTE_PRESENT;
}

inline int pt_entry_is_writable(pt_entry pte)
{
  // return first bit
  return pte & I86_PTE_WRITABLE;
}

inline physical_addr pt_entry_frame(pt_entry pte)
{
  // return the physical address of the frame
  return pte & I86_PTE_FRAME;
}
