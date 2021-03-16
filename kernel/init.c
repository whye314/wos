#include "init.h"
#include "print.h"
#include "interrupt.h"
#include "timer.h"
#include "memory.h"
#include "thread.h"

void init_all(){
    idt_init();
    timer_init();
    mem_init();
    thread_init();
    put_str("initization finished.\n");
}