#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define ONE_KB 1024
#define FOUR_KB 4096
/* init paging */
void init_paging();
extern void execute_paging(int load_addr);
extern void vidmap_paging(int virtual_addr, int physical_addr);
uint32_t vidmap_page_table[ONE_KB] __attribute__((aligned(FOUR_KB)));
extern void disable_paging(int load_addr);
extern void enable_paging(int load_addr);
#endif
