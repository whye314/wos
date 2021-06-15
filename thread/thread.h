#ifndef __THREAD_THREAD_H
#define __THREAD_THREAD_H
#include "memory.h"
#include "stdint.h"
#include "list.h"


#define STACK_MAGIC 0x12345678
#define PG_SIZE 4096

enum task_status{
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIDE
};


//kernel stack before enter interrupt habdler
struct int_stack{
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;

};

//theard stack in switch_to
struct thread_stack{

    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;

    void * eip;//return address for 'ret' in switch_to
    void * unuesd;//if this is a new thread and 'ret' in switch_to has been done, this variable is an dummy return address at schedule.
    //we could change this field "unused" to control PC where this thread finish.
    void * function;
    void * func_args;
};

struct task_struct{
    uint32_t * self_kstack;
    enum task_status status;
    uint8_t priority;
    char name[16];
    uint8_t tricks;//0x19

    uint32_t elapsed_ticks;//0x1c

    struct list_elem general_tag;//offset=0x20
    struct list_elem all_list_tag;

    uint32_t * page;
    struct virtual_addr user_vaddress;

    uint32_t stack_magic;
};



struct task_struct * thread_start(char * name, uint8_t prio, void function(void *), void * args);
void thread_create(struct task_struct * pthread, void function(void *), void * args);
struct task_struct * current_thread();
void thread_block(enum task_status stat);
void thread_unlock(struct task_struct * pthread);
void thread_init(void);
void schedule();
void init_thread(struct task_struct * pthread, char * name, uint8_t prio);

#endif