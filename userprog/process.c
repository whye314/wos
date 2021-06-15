
#include "process.h"
#include "memory.h"
#include "thread.h"
#include "global.h"
#include "debug.h"
#include "tss.h"
#include "console.h"
#include "string.h"
#include "interrupt.h"


extern void int_exit();
extern struct list thread_all_list;
extern struct list thread_ready_list;

int default_prio = 4;

void start_process(void * filename_){

    struct task_struct * current = current_thread();
    current->self_kstack += sizeof(struct thread_stack);
    struct int_stack * proc_stack = (struct int_stack *)current->self_kstack;
    proc_stack->eax = 0;
    proc_stack->ebx = 0;
    proc_stack->ecx = 0;
    proc_stack->edx = 0;
    proc_stack->esi = 0;
    proc_stack->edi = 0;
    proc_stack->esp_dummy = 0;
    proc_stack->ebp = 0;
    proc_stack->gs = 0;
    proc_stack->cs = SELECTOR_U_CODE;
    proc_stack->eip = filename_;
    proc_stack->ds = SELECTOR_U_DATA;
    proc_stack->es = SELECTOR_U_DATA;
    proc_stack->eflags = (EFLAGS_IF_1 | EFLAGS_IOPL_0 | EFLAGS_MBS);
    proc_stack->esp = (void *)((uint32_t)get_a_page(PF_USER, USER_STACK3_ADDR) + PG_SIZE);
    proc_stack->ss = SELECTOR_U_STACK;
    asm volatile("movl %0, %%esp; jmp int_exit"::"g"(proc_stack):"memory");
}

void page_dir_activate(struct task_struct * p_thread){
    uint32_t phy_pagedir_addr = 0x100000;
    if(p_thread->page != NULL){
        phy_pagedir_addr = addr_v2p(p_thread->page);
    }
    asm volatile("mov %0, %%cr3"::"r"(phy_pagedir_addr):"memory");
}

void process_activate(struct task_struct * p_thread){
    ASSERT(p_thread != NULL);
    page_dir_activate(p_thread);
    if(p_thread->page){
        update_esp0(p_thread);
    }
}

uint32_t * create_page_dir(){
    uint32_t * page_dir_vaddr = get_kernel_page(1);
    if(page_dir_vaddr == NULL){
        console_put_str("create_page_dir: get_kernel_page.\n");
        return NULL;
    }
    memcpy((uint32_t *)((uint32_t)page_dir_vaddr + 0x300 * 4), (uint32_t)(0xfffff000 + 0x300 *4), 1024);
    uint32_t new_page_dir_phy = addr_v2p((uint32_t)page_dir_vaddr);
    page_dir_vaddr[1023] = (new_page_dir_phy | PG_US_U | PG_RW_W | PG_P_1);
    return page_dir_vaddr;
}

void create_user_vaddr_bitmap(struct task_struct * user_prog){
    user_prog->user_vaddress.vaddr_start = USER_VADDR_START;
    uint32_t bitmap_c_pg = DIV_ROUND_UP((0xc0000000 - USER_VADDR_START)/(PG_SIZE * 8), PG_SIZE);
    user_prog->user_vaddress.vaddr_bitmap.bits = get_kernel_page(bitmap_c_pg);
    user_prog->user_vaddress.vaddr_bitmap.map_bytes_len = (0xc0000000 - USER_VADDR_START)/(PG_SIZE * 8);
    bitmap_init(&user_prog->user_vaddress.vaddr_bitmap);
}

void process_execute(void * filename, char * name){
    struct task_struct * thread = get_kernel_page(1);
    init_thread(thread, name, default_prio);
    create_user_vaddr_bitmap(thread);
    thread_create(thread, start_process, name);
    thread->page = create_page_dir();

    enum int_status old_status = int_disable();
    ASSERT(!list_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);
    ASSERT(!list_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);
    int_set_status(old_status);
    
}