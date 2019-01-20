#include "paging.h"
#include "types.h"


#define DirectorySize 1024
#define PageSize 4096

#define ATTRIBUTE_2 2
#define ATTRIBUTE_3 3
#define SIZE_4MB 0x400000
#define MASK_83 0x83

#define VM_INDEX 0xB8
#define MASK_87 0x00000087
#define MASK_80 0x00000080
#define MAXDIR 32

#define MASK_7 0x7

static uint32_t Directory[DirectorySize] __attribute__((aligned(PageSize)));
static uint32_t Table[DirectorySize] __attribute__((aligned(PageSize)));


/*
Function: init_paging()
description: This function is called in kernel.c and initializas paging by two of the following method:
    1. Software must load at least one page directory and one page table into physical memory.
    Here we mapped one to one for the kernel (4 meg chunk) and video memory (4KB page) and no other memory.
    2. Control register CR3 (also called the PDBR register) is loaded with the physical base
    address of the page directory
    inputs: none
    outputs: none
*/
void init_paging(){
    int i;
    for(i = 0; i < DirectorySize; i++){
        //  This sets the following attributes to the pages:
        //   Supervisor: kernel-mode only
        //   Read/Write: It can be both read and written 
        //   Present: The page table is not present
        Directory[i] = ATTRIBUTE_2;
       
        Table[i] = (i * PageSize) | ATTRIBUTE_2; // attributes: supervisor level, read/write, not present.
    }
    
    // add page table to page directory
    // attributes: supervisor level, read/write, present
    Directory[0] = ((unsigned int)Table) | ATTRIBUTE_3;
    // map second entry to 4MB for Kernel
    Directory[1] = SIZE_4MB | MASK_83; //attributes: supervisor, present, read/write

    // create a page table entry for video memory 
    Table[VM_INDEX] |= ATTRIBUTE_3; // attributes: supervisor level, read/write, present.
    
    //enable paging
    /* We need to mask %eax during the process */
    asm volatile(
                 "movl %0, %%eax;"
                 "movl %%eax, %%cr3;"
                 "movl %%cr4, %%eax;"
                 "orl $0x00000010, %%eax;"
                 "movl %%eax, %%cr4;"
                 "movl %%cr0, %%eax;"
                 "orl $0x80000000, %%eax;"
                 "movl %%eax, %%cr0;"
                 :                      
                 :"r"(Directory)    
                 :"%eax"                
                 );
}

/*
Function: extern void execute_paging(int load_addr)
description: paging for execute
    inputs: int load_addr
    outputs: none
*/
extern void execute_paging(int load_addr){
    Directory[MAXDIR] = load_addr | MASK_87;
}

/*
Function: extern void vidmap_paging(int virtual_addr, int physical_addr)
description: paging for vidmap
    inputs: int virtual_addr, int physical_addr
    outputs: none
*/
extern void vidmap_paging(int virtual_addr, int physical_addr){
    uint32_t pd_entry = virtual_addr / SIZE_4MB;
    Directory[pd_entry] = (uint32_t)vidmap_page_table | MASK_7;
    vidmap_page_table[0] = physical_addr | MASK_7;
}

/*
Function: disable_paging(int load_addr)
description: disable_paging
    inputs: int load_addr
    outputs: none
*/
extern void disable_paging(int load_addr){
    Directory[MAXDIR] = load_addr | MASK_80;
}

/*
Function: void enable_paging(int load_addr)
description: enable_paging
    inputs: int load_addr
    outputs: none
*/
extern void enable_paging(int load_addr){
    Directory[MAXDIR] = load_addr | MASK_87;
}
