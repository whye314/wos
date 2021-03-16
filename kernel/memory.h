#ifndef __KERNEL_MEMORY_H
#define __KERNEL_MEMORY_H
#include "bitmap.h"
#include "stdint.h"

struct virtual_addr{
    struct bitmap vaddr_bitmap;
    uint32_t vaddr_start;
};

struct mem_pool{
    struct bitmap mem_pool_bitmap;
    uint32_t mem_pool_start;
    uint32_t pool_size;
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


#endif