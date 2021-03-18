#ifndef __THREAD_SYNC_H
#define __THREAD_SYNC_H
#include "stdint.h"
#include "list.h"
#include "thread.h"
struct semaphore{
    uint8_t value;
    struct list waiters;
};

struct lock{
    struct task_struct * holder;
    struct semaphore semaphore;
    uint32_t holder_repeat_n;
};

void lock_release(struct lock * plock);
void lock_acquire(struct lock * plock);
void lock_init(struct lock * plock);
#endif