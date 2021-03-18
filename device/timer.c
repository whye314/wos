#include "time.h"
#include "io.h"
#include "print.h"
#include "thread.h"
#include "debug.h"
#include "interrupt.h"

#define COUNTER0_PORT 0x40 
#define COUNTER0_NO (0 << 6)
#define COUNTER0_MODE (2 << 1)
#define READ_WATCH_LATCH  (3 << 4)
#define PIT_COUNTER_PORT 0x43
#define FREQUENCY0_VALUE 1
#define INPUT_FREQUENCY0_VALUE 1193180

static void int_timer_handler();

uint32_t tricks = 0;

static void frequency_set(uint8_t counter_port, uint8_t counter_no, uint8_t rwl, uint8_t counter_mode, uint16_t counter_value){
    outb(PIT_COUNTER_PORT, (counter_no+READ_WATCH_LATCH+counter_mode));
    outb(counter_port, (uint8_t)(counter_value & 0xffff));
    outb(counter_port, (uint8_t)(counter_value/0x10000));
}

void timer_init(){
    put_str("timer initization start.\n");
    frequency_set(COUNTER0_PORT, COUNTER0_NO, READ_WATCH_LATCH, COUNTER0_MODE, (INPUT_FREQUENCY0_VALUE/FREQUENCY0_VALUE));
    register_handler(0x20, int_timer_handler);
    put_str("timer initization finished.\n");
}

static void int_timer_handler(){
    struct task_struct * current_task = current_thread();
    ASSERT(current_task->stack_magic == STACK_MAGIC);
    current_task->elapsed_ticks++;
    tricks++;
    if(current_task->tricks == 0){
        schedule();
    }
    else{
        current_task->tricks--;
    }
}