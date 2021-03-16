#ifndef __LIB_STRING_H
#define __LIB_STRING_H

#define NULL 0
#include "stdint.h"
void memset(void * dst, uint8_t value, uint32_t size);
void memcpy(void * dst, const void * src, uint32_t size);
int memcmp(const void * a, const void * b, uint32_t size);
char * strcpy(char * dst, const char * src);
uint32_t strlen(const char * str);
int8_t strcmp(const char * a, const char * b);
char * strchr(const char * str, const uint8_t c);
char * strrchr(const char * str, const uint8_t c);
char * strcat(char * dst, const char * src);
uint32_t strchrs(const char * str, const uint8_t c);


#endif