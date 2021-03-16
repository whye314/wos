#ifndef __KERNEL_DEUBG_H
#define __KERNEL_DEBUG_H

void painc(char * filename, int line, const char * func, const char * condition);

#define PANIC(...) painc(__FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef NDEBUG

    #define ASSERT(CONDITION) ((void)0)
#else
    #define ASSERT(CONDITION) \
    if(CONDITION){}\
    else{\
        PANIC(#CONDITION);\
    }


#endif

#endif