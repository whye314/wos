#ifndef __DEVICE_IOQUEUE_H
#define __DEVICE_IOQUEUE_H

#define buf_size 64

#include "sync.h"
#include "thread.h"
#include "stdint.h"

extern struct ioqueue kbd_buf;

struct ioqueue{
    struct lock lock;
    struct task_struct * comsumer;
    struct task_struct * producer;

    uint8_t buf[buf_size];
    uint32_t head;
    uint32_t tail;

};

void ioq_write(struct ioqueue * queue, uint8_t c);
uint8_t ioq_read(struct ioqueue * queue);
uint8_t ioq_isempty(struct ioqueue * queue);
uint8_t ioq_isfull(struct ioqueue * queue);
void ioqueue_init(struct ioqueue * queue);




#endif