#ifndef __USERPROG_PROCESS_H
#define __USERPROG_PROCESS_H
#include "memory.h"
#include "thread.h"

#define USER_STACK3_ADDR (0xc0000000 - 0x1000)
#define USER_VADDR_START 0x08048000

void process_execute(void * filename, char * name);
void process_activate(struct task_struct * p_thread);

#endif