#ifndef __LIB_KERNEL_PRINT_H
#define __LIB_KERNEL_PRINT_H
#include "stdint.h"
void put_char(uint8_t n);
void put_str(uint8_t * s);
void put_int(uint32_t num);
#endif