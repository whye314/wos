#ifndef __USERPROG_TSS_H
#define __USERPROG_TSS_H

void tss_init();
void update_esp0(struct task_struct * pthread);

#endif