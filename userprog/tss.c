#include "thread.h"
#include "tss.h"
#include "global.h"
#include "string.h"



#define PG_SIZE 4096

struct tss{
    uint32_t old_tss;
    uint32_t * esp0;
    uint32_t ss0;
    uint32_t * esp1;
    uint32_t ss1;
    uint32_t * esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t io_base;
};

static struct tss tss;

void update_esp0(struct task_struct * pthread){
    tss.esp0 = (uint32_t *)((uint32_t)pthread + PG_SIZE);
}

static struct desc make_desc(uint32_t addr, uint32_t limit, uint8_t attr_low, uint8_t attr_high){
    struct desc pdesc;
    pdesc.base_address_low = (addr && 0xffff);
    pdesc.limit_low = (limit & 0xffff);
    pdesc.base_address_middle = ((addr >> 16) && 0xff);
    pdesc.attr_low = attr_low;
    pdesc.limit_high_attr_high = ((limit >> 16) & 0xf) + (attr_high);
    pdesc.base_address_high = (addr >> 24);
    return pdesc;
    
}

void tss_init(){

    put_str("tss initization start.\n");

    struct tss tss;
    memset(&tss, 0, sizeof(tss));
    tss.ss = SELECTOR_STACK;
    tss.io_base = sizeof(tss);

    *(struct desc *)(0x903 + 0x5 * 8) = make_desc(&tss, sizeof(tss), TSS_ATTR_LOW, TSS_ATTR_HIGH);


    //user code
    *(struct desc *)(0x903 + 0x6 * 8) = make_desc(0, 0xfffff, GDT_ATTR_LOW_CODE_DPL3, GDT_ATTR_HIGH);
    //data
    *(struct desc *)(0x903 + 0x7 * 8) = make_desc(0, 0xfffff, GDT_ATTR_LOW_DATA_DPL3, GDT_ATTR_HIGH);
    //stack
    *(struct desc *)(0x903 + 0x8 * 8) = make_desc(0, 0, GDT_ATTR_LOW_STACK_DPL3, GDT_ATTR_HIGH);

    uint64_t gdt = (0x903 << 16) + (9 * 8 - 1);
    
    asm volatile("lgdt %0"::"m"(gdt));
    asm volatile ("ltr %w0"::"a"(TSS_SELECTOR));


}