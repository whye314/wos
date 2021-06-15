#include "memory.h"
#include "stdint.h"
#include "print.h"
#include "bitmap.h"
#include "string.h"
#include "debug.h"
#include "sync.h"

#define PG_SIZE 4096
#define MEM_BITMAP_BASE 0xc009a000
#define KERNEL_HEAP_START 0xc0100000

#define GET_PTE(address) (0xffc00000 + (address >> 12)*4)
#define GET_PDE(address) (0xfffff000 + (address >> 22)*4)

struct mem_pool{
    struct bitmap mem_pool_bitmap;
    uint32_t mem_pool_start;
    uint32_t pool_size;
    struct lock lock;
};



struct virtual_addr kernel_vaddr;
struct mem_pool kernel_pool, user_pool;




void mem_pool_init(uint32_t all_pool_size){
    put_str("memory pool initization start.\n");
    uint32_t used_mem = 0x100000 + PG_SIZE*256;
    uint32_t all_page_free = (all_pool_size - used_mem) / PG_SIZE;

    uint32_t kernel_pool_size = ((all_page_free >> 4) << 3);
    uint32_t user_pool_size = all_page_free - kernel_pool_size;

    uint32_t kbm_length = kernel_pool_size >> 3;
    uint32_t ubm_length = user_pool_size >> 3;

    kernel_pool.pool_size = kernel_pool_size * PG_SIZE;
    kernel_pool.mem_pool_start = used_mem;
    kernel_pool.mem_pool_bitmap.map_bytes_len = kbm_length;
    kernel_pool.mem_pool_bitmap.bits = (uint8_t *)(MEM_BITMAP_BASE);

    user_pool.pool_size = user_pool_size * PG_SIZE;
    user_pool.mem_pool_start = used_mem + (kernel_pool_size * PG_SIZE);
    user_pool.mem_pool_bitmap.map_bytes_len = ubm_length;
    user_pool.mem_pool_bitmap.bits = (uint8_t *)(MEM_BITMAP_BASE + kbm_length);

    lock_init(&kernel_pool.lock);
    lock_init(&user_pool.lock);

    put_str("kernel memory pool start address: ");
    put_int(kernel_pool.mem_pool_start);
    put_str("\n");
    put_str("bitmap start address: ");
    put_int(kernel_pool.mem_pool_bitmap.bits);
    put_str("\n");
    put_str("user memory pool start address: ");
    put_int(user_pool.mem_pool_start);
    put_str("\n");
    put_str("bitmap start address: ");
    put_int(user_pool.mem_pool_bitmap.bits);
    put_str("\n");

    bitmap_init(&kernel_pool.mem_pool_bitmap);
    bitmap_init(&user_pool.mem_pool_bitmap);

    kernel_vaddr.vaddr_bitmap.map_bytes_len = kbm_length;
    kernel_vaddr.vaddr_bitmap.bits = MEM_BITMAP_BASE + kbm_length + ubm_length;
    kernel_vaddr.vaddr_start = KERNEL_HEAP_START;

    bitmap_init(&kernel_vaddr.vaddr_bitmap);
    put_str("memory pool initization finished.\n");

}

void mem_init(){
    put_str("memory initization start.\n");
    uint32_t all_bytes_memory = *(uint32_t*)(0xb00);
    mem_pool_init(all_bytes_memory);
    put_str("memory initization finished.\n");
}



static void * get_vaddr(enum pool_flags pf, uint32_t n){
    uint32_t start_addr = NULL;
    if(pf == PF_KERNEL){
        start_addr = bitmap_scan(&kernel_vaddr, n);
        if(start_addr == -1) return NULL;
        while(n-->0){
            bitmap_set(&kernel_vaddr.vaddr_bitmap, start_addr+n, 1);
        }
        return (void *)(kernel_vaddr.vaddr_start + start_addr * PG_SIZE);
    }
    else{
        //user_virtual_address
        struct task_struct * current = current_thread();
        start_addr = bitmap_scan(&current->user_vaddress.vaddr_bitmap, n);
        if(start_addr == -1) return NULL;
        while(n-->0){
            bitmap_set(&current->user_vaddress.vaddr_bitmap, start_addr+n, 1);
        }
        ASSERT((uint32_t)start_addr < 0xc0000000 - PG_SIZE);
        return (void *)(current->user_vaddress.vaddr_start + start_addr * PG_SIZE);
    }
}

static void * palloc(struct mem_pool * pool){
    uint32_t address = bitmap_scan(pool, 1);
    if(address == -1) return NULL;
    bitmap_set(pool, address, 1);
    return (void *)(pool->mem_pool_start + address * PG_SIZE);
}

void * set_page_table(void * virtual_address, void * phy_address){
    uint32_t * pte = (uint32_t*)GET_PTE((uint32_t)virtual_address);
    uint32_t * pde = (uint32_t*)GET_PDE((uint32_t)virtual_address);
    if(*pde & 0x1){
        *pte = phy_address + PG_P_1 + PG_RW_W + PG_US_U;
    }else{
        uint32_t * new_pg = (uint32_t *)palloc(&kernel_pool);
        //uint32_t * new_pde = (uint32_t *)GET_PDE((uint32_t)new_pg);
        uint32_t * new_pte = (uint32_t *)GET_PTE((uint32_t)((uint32_t)virtual_address & 0xffc00000));

        //memset(new_pde, 0, PG_SIZE);
        *pde = new_pg + PG_P_1 + PG_RW_W + PG_US_U;
        memset(new_pte, 0, PG_SIZE);
        *pte = phy_address + PG_P_1 + PG_RW_W + PG_US_U;
    }
}

void * malloc_page(enum pool_flags pf, uint32_t n){
    ASSERT(n > 0 && n < 3840);
    void * vaddress = get_vaddr(pf, n);
    if(!vaddress) return NULL;
    struct mem_pool pool = pf == 1 ? kernel_pool : user_pool;
    while(n-->0){
        void * paddress = palloc(&pool);
        if(paddress == NULL) return NULL;//need gc
        set_page_table((vaddress + n*PG_SIZE), paddress);
    }
    return vaddress;
}

void * get_kernel_page(uint32_t n){
    lock_acquire(&kernel_pool.lock);
    void * vaddress = malloc_page(PF_KERNEL, n);
    if(vaddress != NULL){
        memset(vaddress, 0, n*PG_SIZE);
    }
    lock_release(&kernel_pool.lock);
    return vaddress;
}

void * get_user_page(uint32_t n){
    lock_acquire(&user_pool.lock);
    void * vaddr = malloc_page(PF_USER, n);
    memset(vaddr, 0, n * PG_SIZE);
    lock_release(&user_pool.lock);
    return vaddr;
}

void * get_a_page(enum pool_flags pf, uint32_t vaddr){
    struct mem_pool * pool = pf==PF_KERNEL ? &kernel_pool : &user_pool;
    lock_acquire(&pool->lock);
    struct task_struct * current = current_thread();
    uint32_t bit_idx = -1;
    if(current->page != NULL && pf == PF_USER){
        bit_idx = (vaddr - current->user_vaddress.vaddr_start)/PG_SIZE;
        ASSERT(bit_idx > 0);
        bitmap_set(&current->user_vaddress.vaddr_bitmap, bit_idx, 1);
    }
    else if(current->page == NULL && pf == PF_KERNEL){
        bit_idx = (vaddr - kernel_vaddr.vaddr_start)/PG_SIZE;
        ASSERT(bit_idx > 0);
        bitmap_set(&kernel_vaddr.vaddr_bitmap, bit_idx, 1);
    }
    else{
        PANIC("get_a_page: error.\n");
    }
    void * paddr = palloc(pool);
    if(paddr == NULL) return NULL;
    set_page_table(vaddr, paddr);
    lock_release(&pool->lock);
    return vaddr;
}

uint32_t addr_v2p(uint32_t vaddr){
    uint32_t * pte = (uint32_t *)(0xffc00000 + ((vaddr & 0xfffff000) >> 10));
    return (uint32_t)((vaddr & 0xfff) + (*pte & 0xfffff000));
}