#include "ioqueue.h"
#include "debug.h"
#include "interrupt.h"
#include "string.h"
#include "print.h"


struct ioqueue kbd_buf;

void ioqueue_init(struct ioqueue * queue){
    lock_init(&queue->lock);
    queue->producer = NULL;
    queue->tail = NULL;
}

static uint32_t next(uint32_t c){
    return (c+1)%buf_size;
}

uint8_t ioq_isempty(struct ioqueue * queue){
    ASSERT(int_get_status() == INT_OFF);
    return queue->head == queue->tail;
}

uint8_t ioq_isfull(struct ioqueue * queue){
    ASSERT(int_get_status() == INT_OFF);
    return next(queue->head) == queue->tail;
}

static void ioq_wait(struct task_struct ** waiter){
    ASSERT(waiter != NULL);
    //put_str("R");
    ASSERT(*waiter == NULL);
    ASSERT(waiter != NULL && *waiter == NULL);
    *waiter = current_thread();
    //put_str("io wait.\n");
    thread_block(TASK_BLOCKED);
}

static void ioq_wakeup(struct task_struct ** waiter){
    ASSERT(*waiter != NULL);
    //put_str("io wakeup. ");put_str(current_thread()->name);put_str("\n");
    thread_unlock(*waiter);
    *waiter = NULL;
}


void ioq_write(struct ioqueue * queue, uint8_t c){
    ASSERT(int_get_status() == INT_OFF);
    while(ioq_isfull(queue)){
        //lock_acquire(&queue->lock);
        //put_str("\nproducer wait.\n");
        //ioq_wait(&queue->producer);
        //lock_release(&queue->lock);
        queue->tail = next(queue->tail);
    }

    //lock_acquire(&queue->lock);
    queue->buf[queue->head] = c;
    queue->head = next(queue->head);
    if(queue->comsumer != NULL){
        ioq_wakeup(&queue->comsumer);
    }
    //lock_release(&queue->lock);
}

uint8_t ioq_read(struct ioqueue * queue){
    ASSERT(int_get_status() == INT_OFF);
    while (ioq_isempty(queue))
    {
        lock_acquire(&queue->lock);
        ioq_wait(&queue->comsumer);
        lock_release(&queue->lock);
    }
    //lock_acquire(&queue->lock);
    uint8_t c = queue->buf[queue->tail];
    queue->tail = next(queue->tail);
    if(queue->producer != NULL){
        ioq_wakeup(&queue->producer);
    }    
    //lock_release(&queue->lock);
    return  c;

    
}