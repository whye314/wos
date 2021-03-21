#include "print.h"
#include "init.h"
#include "debug.h"
#include "interrupt.h"
#include "memory.h"
#include "thread.h"
#include "console.h"
#include "ioqueue.h"

void print(void *a);

int main(void){
    put_str("kernel running at address: ");
    put_int(0xc0001500);
    put_str("\n");
    init_all();//0xc0001552
    put_str("before int_enable.\n");
    int_enable();

    thread_start("print", 100, print, "0 ");
    put_str("thread 0 start.\n");
    thread_start("print", 100, print, "1 ");
    put_str("thread 1 start.\n");
    thread_start("print", 100, print, "2 ");
    put_str("thread 2 start.\n");

    
    while(1) ;//console_put_str("m ");
    return 0;
}

void print(void * a){
    
    while(1){
        enum int_status old_status = int_disable();
        //if(!ioq_isempty(&kbd_buf)){
            //console_put_str(a);
            put_str(a);
            put_char(ioq_read(&kbd_buf));
            //console_put_char(ioq_read(&kbd_buf));
        //}

        int_set_status(old_status);
        uint32_t s = 10000000;
        asm volatile("l:loop l"::"c"(s));
    }
}