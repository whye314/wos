#include "thread.h"
#include "memory.h"
#include "string.h"
#include "interrupt.h"
#include "debug.h"
#include "print.h"


extern void switch_to(struct task_struct * current, struct task_struct * next);


struct task_struct * main_thread;
struct list thread_all_list;
struct list thread_ready_list;




static void kernel_thread(void * func(void*), void * args){
    int_enable();
    func(args);
}

static void thread_stack(struct task_struct * pthread, void function(void *), void * args){
    pthread->self_kstack -= sizeof(struct int_stack);
    pthread->self_kstack -= sizeof(struct thread_stack);
    struct thread_stack * kstack = (struct thread_stack *)pthread->self_kstack;
    kstack->eip = kernel_thread;
    kstack->function = function;
    kstack->func_args = args;
    kstack->ebp = 0; kstack->ebx = 0; kstack->edi = 0; kstack->esi = 0;
}

static void init_thread(struct task_struct * pthread, char * name, uint8_t prio){
    memset(pthread, 0, sizeof(*pthread));
    strcpy(pthread->name, name);
    pthread->priority = prio;
    pthread->stack_magic = STACK_MAGIC;
    if(pthread == main_thread)
    {
        pthread->status = TASK_RUNNING;
    }
    else{
        pthread->status = TASK_READY;
    }
    pthread->self_kstack = (uint32_t *)((uint32_t)pthread + PG_SIZE);
}

struct task_struct * thread_start(char * name, uint8_t prio, void function(void *), void * args){
    struct task_struct * thread = get_kernel_page(1);
    init_thread(thread, name, prio);
    thread_stack(thread, function, args);
    ASSERT(!list_find(&thread_all_list, &thread->all_list_tag));
    list_append(&thread_all_list, &thread->all_list_tag);
    ASSERT(!list_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);

    // asm volatile("movl %0, %%esp; pop %%ebx; pop %%edi; pop %%esi; pop %%ebp;"::"g"(thread->self_kstack):"memory");
    
    // asm volatile("movl %0, %%eax; ret"::"a"(thread));
    return thread;
}

static void main_thread_start(){
    main_thread = current_thread();
    init_thread(main_thread, "main", 31);
    ASSERT(!list_find(&thread_all_list, &main_thread->all_list_tag));
    list_append(&thread_all_list, &main_thread->all_list_tag);
}

struct task_struct * current_thread(){
    uint32_t esp;
    asm volatile("movl %%esp, %0":"=g"(esp));
    return (struct task_struct *)(esp & 0xfffff000);
}

void schedule(){
    ASSERT(int_get_status() == INT_OFF);
    struct task_struct * current_task = current_thread();
    if(current_task->status == TASK_RUNNING){
        list_append(&thread_ready_list, &current_task->general_tag);
        current_task->tricks = current_task->priority;
        current_task->status = TASK_READY;
    }else{}
    ASSERT(!list_isempty(&thread_ready_list));

    struct task_struct * next = get_address_from_member(struct task_struct, general_tag, list_pop(&thread_ready_list));
    next->status = TASK_RUNNING;
    put_str("\nschedule.\n");
    switch_to(current_task, next);


}

void thread_init(void){
    put_str("thread initization start.\n");
    list_init(&thread_all_list);
    list_init(&thread_ready_list);
    main_thread_start();
    put_str("thread initization finish.\n");
}
