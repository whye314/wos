#include "sync.h"
#include "string.h"
#include "interrupt.h"
#include "debug.h"

void sema_init(struct semaphore * psema, uint8_t value){
    psema->value = value;
    list_init(&psema->waiters);
}

void lock_init(struct lock * plock){
    plock->holder = NULL;
    plock->holder_repeat_n = 0;
    sema_init(&plock->semaphore, 1);
}

void sema_down(struct semaphore * psema){
    enum int_status old_status = int_disable();
    while(psema->value == 0){
        ASSERT(!list_find(&psema->waiters, &current_thread()->general_tag));
        list_append(&psema->waiters, &current_thread()->general_tag);
        thread_block(TASK_BLOCKED);
    }
    psema->value --;
    int_set_status(old_status);
}

void sema_up(struct semaphore * psema){
    enum int_status old_status = int_disable();
    ASSERT(psema->value == 0);
    if(!list_isempty(&psema->waiters)){
        struct task_struct * thread_block = get_address_from_member(struct task_struct, general_tag, list_pop(&psema->waiters));
        thread_unlock(thread_block);
    }
    psema->value++;
    int_set_status(old_status);
}


void lock_acquire(struct lock * plock){
    enum int_status old_status = int_disable();
    if(plock->holder != current_thread()){
        sema_down(&plock->semaphore);
        plock->holder = current_thread();
        ASSERT(plock->holder_repeat_n == 0);
        plock->holder_repeat_n = 1;
    }else{
        plock->holder_repeat_n++;
    }
    int_set_status(old_status);
}

void lock_release(struct lock * plock){
    ASSERT(plock->holder == current_thread());
    enum int_status old_status = int_disable();
    if(plock->holder_repeat_n > 1){
        plock->holder_repeat_n -- ;
    }
    else{
        plock->holder = NULL;
        plock->holder_repeat_n = 0;
        sema_up(&plock->semaphore);
    }
    int_set_status(old_status);

}
