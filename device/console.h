#ifndef __DEVIDE_CONSOLE_H
#define __DEVICE_CONSOLE_H
#include "stdint.h"

void console_init();
void console_put_str(char * str);
void console_put_int(uint32_t n);
void console_put_char(uint8_t n);

#endif