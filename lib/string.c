#include "string.h"
#include "stdint.h"
#include "debug.h"

//while(size--) *(dst+size)=value
void memset(void * dst, uint8_t value, uint32_t size){
    ASSERT(dst != NULL);
    while(size--) *(uint8_t *)(dst + size) = value;
}

//src -> dst
void memcpy(void * dst, const void * src, uint32_t size){
    ASSERT(src != NULL && dst != NULL);
    while(size--) *(uint8_t*)(dst++) = *(uint8_t*)(src++);
}

int memcmp(const void * a, const void * b, uint32_t size){
    ASSERT(a != NULL && b != NULL);
    while (size--)
    {
        if(*(uint8_t*)(a++) > *(uint8_t*)(b++)) return 1;
        if(*(uint8_t*)(a++) < *(uint8_t*)(b++)) return -1;
    }
    return 0;
    
}

char * strcpy(char * dst, const char * src){
    ASSERT(dst != NULL && src != NULL);
    char * r = dst;
    while(*(dst++) = *(src++));
    return r;
}

uint32_t strlen(const char * str){
    ASSERT(str != NULL);
    uint32_t r = 0;
    while(*(str++)) r++;
    return r;
}

int8_t strcmp(const char * a, const char * b){
    ASSERT(a != NULL && b != NULL);
    while ((*a!=NULL) && *(a++)==*(b++));
    return *(a-1) < *(b-1) ? -1 : *(a-1) > *(b-1);
    
}

char * strchr(const char * str, const uint8_t c){
    ASSERT(str != NULL);
    while(*str && *(str++) != c);
    return (*str)? (char *)(str-1) : NULL;
}

char * strrchr(const char * str, const uint8_t c){
    ASSERT(str != NULL);
    char * r = NULL;
    while(*str){
        if(*(str++) == c) r = (char *)(str-1);
    }
    return r;
}

char * strcat(char * dst, const char * src){
    ASSERT(dst != NULL && src != NULL);
    char * r = dst;
    while(*(dst++));
    dst--;
    while(*(src++)) *(dst++) = *(src-1);
    return r;
}

uint32_t strchrs(const char * str, const uint8_t c){
    ASSERT(str != NULL);
    uint32_t r = 0;
    while(*(str)){
        if(*(str++) == c) r++;
    }
    return r;
}
