#ifndef __KERNEL_INTERRUPT_H
#define __KERNLE_INTERRUPT_H

#include "stdint.h"

typedef void* int_handler;
void idt_init();

enum int_status{
    INT_OFF,
    INT_ON
};

enum int_status int_get_status();
enum int_status int_set_status(enum int_status status);
enum int_status int_enable();
enum int_status int_disable();
void register_handler(uint8_t vec_no, int_handler func);

#endif