#ifndef __KERNEL_MEMORY_H
#define __KERNEL_MEMORY_H
#include "bitmap.h"
#include "stdint.h"
// #include "sync.h"

struct virtual_addr{
    struct bitmap vaddr_bitmap;
    uint32_t vaddr_start;
};



enum pool_flags{
    PF_KERNEL = 1,
    PF_USER = 2
};

#define PG_P_0 0
#define PG_P_1 1
#define PG_RW_R (0 << 1)
#define PG_RW_W (1 << 1)
#define PG_US_U (1 << 2)
#define PG_US_S (0 << 2)

void mem_init();
void * get_kernel_page(uint32_t n);
void * get_user_page(uint32_t n);
uint32_t addr_v2p(uint32_t vaddr);
void * get_a_page(enum pool_flags pf, uint32_t vaddr);


#endif