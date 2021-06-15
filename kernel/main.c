#include "print.h"
#include "init.h"
#include "debug.h"
#include "interrupt.h"
#include "memory.h"
#include "thread.h"
#include "console.h"
#include "ioqueue.h"
#include "process.h"

int test_var_a = 0, test_var_b = 0;

void print(void *a);
void up_a();
void up_b();
void kt_a();
void kt_b();

int main(void){
    put_str("kernel running at address: ");
    put_int(0xc0001500);
    put_str("\n");
    init_all();//0xc0001552
    

    // thread_start("print", 100, print, "0 ");
    // put_str("thread 0 start.\n");
    // thread_start("print", 100, print, "1 ");
    // put_str("thread 1 start.\n");
    // thread_start("print", 100, print, "2 ");
    // put_str("thread 2 start.\n");
    
    
    process_execute(up_a, "u_a");
    put_str("process a start.\n");
    process_execute(up_b, "u_b");
    put_str("process b start.\n");

    thread_start("k_a", 31, kt_a, 0);
    put_str("thread a start.\n");
    thread_start("k_b", 31, kt_b, 0);
    put_str("thread b start.\n");

    
    int_enable();
    put_str("after int_enable.\n");

    
    while(1) console_put_str("m ");
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

void up_a(){
    while(1){
        test_var_a ++;
    }
}
void up_b(){
    while(1){
        test_var_b ++;
    }
}

void kt_a(){
    while(1){
        console_put_str(" v_a: ");
        console_put_int(test_var_a);
    }
}

void kt_b(){
    while(1){
        console_put_str(" v_b: ");
        console_put_int(test_var_b);
    }
}