#include "interrupt.h"
#include "global.h"
#include "stdint.h"
#include "io.h"


#define IDT_DESC_CNT 0x21

extern void put_str(void *);
extern void put_int(uint32_t);

struct gate_desc{
    uint16_t offset_low;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset_high;
};

static void make_idt_desc(struct gate_desc * p_gdesc, uint16_t attr, int_handler function);
static struct gate_desc idt[IDT_DESC_CNT];

extern int_handler int_entry_table[IDT_DESC_CNT];
char *int_name[IDT_DESC_CNT];
int_handler idt_table[IDT_DESC_CNT];

static void make_idt_desc(struct gate_desc * p_gdesc, uint16_t attr, int_handler function){
    p_gdesc->offset_low = (uint32_t)function & 0x0000ffff;
    p_gdesc->selector = SELECTOR_CODE;
    p_gdesc->attr = attr;
    p_gdesc->offset_high = (uint32_t)function >> 16;
}

static void idt_desc_init(void){//c0001609
    for(int i = 0; i < IDT_DESC_CNT; i++){
        make_idt_desc(&idt[i], IDT_DESC_ATTR_DPL0, int_entry_table[i]);
    }
    put_str("idt description initization finfished.\n");
}

static void pic_init(){
    outb(0x20, 0x11);
    outb(0x21, 0x20);

    outb(0x21, 0x04);
    outb(0x21, 0x01);

    outb(0xa0, 0x11);
    outb(0xa1, 0x28);

    outb(0xa1, 0x02);
    outb(0xa1, 0x01);

    outb(0x21, 0x02);
    outb(0xa1, 0x01);

    put_str("pic initization finished.\n");
}





static void general_int_hadnler(uint8_t vec_n){
    if(vec_n == 0x27 || vec_n == 0x2a ){
        return;
    }
    put_str("int vector: ");
    put_int((uint32_t)vec_n);
    put_str(" ");
    put_str(int_name[vec_n]);
    put_str("\n");
    while(1);
}
static void exception_init(){
    for(int i=0; i < IDT_DESC_CNT; i++){
        idt_table[i] = general_int_hadnler;
        int_name[i] = "unknown";
    }
    int_name[0] = "#DE Divide Error";
    int_name[1] = "#DB Debug Exception";
    int_name[2] = "NMI Interrupt";
    int_name[3] = "#BP Breakpoint Exception";
    int_name[4] = "#OF Overflow Exception";
    int_name[5] = "#BR BOUND Range Exceeded Exception";
    int_name[6] = "#UD Invalid Opcode Exception";
    int_name[7] = "#NM Device Not Available Exception";
    int_name[8] = "#DF Double Fault Exception";
    int_name[9] = "Coprocesser Segment Overrun";
    int_name[10] = "#TS Invalid TSS Exception";
    int_name[11] = "#NP Segment Not Persent";
    int_name[12] = "#SS Stack Fault Exception";
    int_name[13] = "#GP General Protecetion Exception";
    int_name[14] = "#PF Page-Fault Exception";
    //int_name[15]
    int_name[16] = "#MF x87 FPU Floating-Point Error";
    int_name[17] = "#AC Alignment Check Exception";
    int_name[18] = "#MC Machine-Check Exception";
    int_name[19] = "#XF SIMD Floating-Point Exception";
}



void idt_init(void){
    put_str("idt initization start.\n");
    idt_desc_init();
    exception_init();
    pic_init();//init 8259A

    uint64_t idt_ptr;
    idt_ptr = sizeof(idt) - 1 | ((uint64_t)((uint64_t)idt << 16));
    asm volatile("lidt %0": : "m"(idt_ptr));
    put_str("idt description finished.\n");
}

#define EFLAGS_IF (0x1<<9)
#define GET_EFLAGS(EFLAG_VAR) asm volatile("pushfl; popl %0" : "=g"(EFLAG_VAR))

enum int_status int_get_status(){
    uint32_t eflags = 0;
    GET_EFLAGS(eflags);
    return (eflags & EFLAGS_IF)? INT_ON: INT_OFF;
}



enum int_status int_set_status(enum int_status status){
    (status & INT_ON) ? int_enable(): int_disable();
}


enum int_status int_enable(){
    enum int_status old_status;
    if(old_status = int_get_status()){
        return old_status;
    }
    else
    {
        asm("sti");
        return old_status;
    }
}

enum int_status int_disable(){
    enum int_status old_status;
    if(old_status = int_get_status()){
        asm("cli");
        return old_status;
    }
    else
    {
        return old_status;
    }
}

void register_handler(uint8_t vec_no, int_handler func){
    idt_table[vec_no] = func;
}