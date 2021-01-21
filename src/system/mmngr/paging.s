.section .text

stuff:
  # page directory table
  .set PAGE_DIR, 0x9C000
  # 0th page table. Address must be 4KB aligned
  .set PAGE_TABLE_0, 0x9D000
  # 768th page table, because 768*4096=3gb. Address must be 4KB aligned
  .set PAGE_TABLE_768, 0x9E000
  # each page table has 1024 entries
  .set PAGE_TABLE_ENTRIES,	1024
  # each page is 4096 bytes long
  .set PAGE_SIZE, 4096
  .set PTE_PDE_SIZE, 4
  .set PAGE_FOR_3GB, 768

  # attributes (page is present : bit 0 is set to 1, page is writable : bit 1 is set to 1, supervisor mode : bit 2 is set to 0)
  .set ATTRIBUTE, 3

.global _enable_paging
.type _enable_paging, @function
_enable_paging:
  pusha

  # set all the entries in the first page table
  mov $PAGE_TABLE_ENTRIES, %ecx # loop for 1024 times
  mov $PAGE_TABLE_0, %eax
  mov $ATTRIBUTE, %ebx
.first_table_loop:
  mov %ebx, (%eax) # save attribute and physical address at address
  add $PTE_PDE_SIZE, %eax # move 4 bytes to the next entry
  add $PAGE_SIZE, %ebx # add 4096 bytes to the address of the physical address of the page
  loop .first_table_loop

  # set all the entries in the 768th page table
  mov $PAGE_TABLE_ENTRIES, %ecx # loop for 1024 times
  mov $PAGE_TABLE_768, %eax
  mov $ATTRIBUTE, %ebx
  add $0x100000, %ebx # physical address starts 1MB
.768_table_loop:
  mov %ebx, (%eax) # save attribute and physical address at address
  add $PTE_PDE_SIZE, %eax # move 4 bytes to the next entry
  add $PAGE_SIZE, %ebx # add 4096 bytes to the address of the physical address of the page
  loop .768_table_loop


  # add first page table to page directory
  mov $PAGE_TABLE_0, %ebx
  add $ATTRIBUTE, %ebx # physical address and attributes of first page table
  mov $PAGE_DIR, %eax
  mov %ebx, (%eax)
  # get address of 768th page
  mov $PTE_PDE_SIZE, %eax
  mov $PAGE_FOR_3GB, %ebx
  mull %ebx # multiply size of pde with 768
  add $PAGE_DIR, %eax # starting at page directory address
  # again add page table
  mov $PAGE_TABLE_768, %ebx
  add $ATTRIBUTE, %ebx # physical address and attributes of 768th page table
  mov %ebx, (%eax)


  # set cr3 and cr0
  mov $PAGE_DIR, %eax
  mov %eax, %cr3
  mov %cr0, %eax
  or $0x80000000, %eax
  mov %eax, %cr0


  popa
  ret
