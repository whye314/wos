#include "print.h"
#include "init.h"
#include "debug.h"
#include "interrupt.h"
#include "memory.h"
#include "thread.h"

void print(void *a);

int main(void){
    put_str("kernel running at address: ");
    put_int(0xc0001500);
    put_str("\n");
    init_all();//0xc0001552
    int_enable();

    thread_start("print", 2, print, "0");
    put_str("thread 0 start.\n");
    thread_start("print", 5, print, "1");
    put_str("thread 1 start.\n");
    thread_start("print", 2, print, "2");
    put_str("thread 2 start.\n");


    while(1);
    return 0;
}

void print(void * a){
    
    while(1){
        int_disable();
        put_str(a);
        int_enable();
    }
}