#ifndef __KERNEL_GLOBAL_H
#define __KERNEL_GLOBAL_H
#include "stdint.h"

#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3

#define TI_GDT (0 << 2)
#define TI_LDT (1 << 2)

#define SELECTOR_CODE ((0x1 << 3) + TI_GDT + RPL0)
#define SELECTOR_DATA ((0x2 << 3) + TI_GDT + RPL0)
#define SELECTOR_STACK ((0x3 << 3) + TI_GDT + RPL0)
#define SELECTOR_VIDEO ((0x4 << 3) + TI_GDT + RPL0)



#define IDT_DESC_P (1 << 15)
#define IDT_DESC_DPL0 (0 << 13)
#define IDT_DESC_RPL3 (3 << 13)
#define IDT_DESC_S (0 << 12)
#define IDT_DESC_TYPE_32 (0xe << 8)




#define IDT_DESC_ATTR_DPL0 (IDT_DESC_P + IDT_DESC_DPL0 + IDT_DESC_S + IDT_DESC_TYPE_32)
#define IDT_DESC_ATTR_DPL3 (IDT_DESC_P + IDT_DESC_DPL3 + IDT_DESC_S + IDT_DESC_TYPE_32)


//TSS
#define TSS_DESC_D (0 << 22)
#define TSS_DESC_L (0 << 21)
#define TSS_DESC_G_4K (1 << 23)
#define TSS_DESC_AVL ( 0 << 20)
#define TSS_DESC_P (1 << 15)
#define TSS_DESC_DPL0 (0 << 13)
#define TSS_DESC_S (0 << 12)
#define TSS_DESC_TYPE (0b1001 << 8)

#define TSS_ATTR_LOW ((TSS_DESC_TYPE + TSS_DESC_S + TSS_DESC_DPL0 + TSS_DESC_P) >> 8)
#define TSS_ATTR_HIGH ((TSS_DESC_AVL + TSS_DESC_D + TSS_DESC_L + TSS_DESC_G_4K) >> 16)


//GDT
#define GDT_DESC_D_32 (1 << 22)
#define GDT_DESC_B (0 << 22)
#define GDT_DESC_L (1 << 21)
#define GDT_DESC_G_4K (1 << 23)
#define GDT_DESC_G_B (0 << 23)
#define GDT_DESC_AVL ( 0 << 20)
#define GDT_DESC_P (1 << 15)
#define GDT_DESC_DPL0 (0 << 13)
#define GDT_DESC_DPL1 (1 << 13)
#define GDT_DESC_DPL2 (2 << 13)
#define GDT_DESC_DPL3 (3 << 13)
#define GDT_DESC_S (1 << 12)
#define GDT_DESC_TYPE_CODE (0b1000 << 8)
#define GDT_DESC_TYPE_DATA (0b0010 << 8)
#define GDT_DESC_TYPE_STACK (0b0110 << 8)


#define GDT_ATTR_LOW_CODE_DPL3 ((GDT_DESC_TYPE_CODE + GDT_DESC_S + GDT_DESC_DPL3 + GDT_DESC_P) >> 8)
#define GDT_ATTR_LOW_DATA_DPL3 ((GDT_DESC_TYPE_DATA + GDT_DESC_S + GDT_DESC_DPL3 + GDT_DESC_P) >> 8)
#define GDT_ATTR_LOW_STACK_DPL3 ((GDT_DESC_TYPE_STACK + GDT_DESC_S + GDT_DESC_DPL3 + GDT_DESC_P) >> 8)
#define GDT_ATTR_HIGH ((GDT_DESC_AVL + GDT_DESC_D_32 + GDT_DESC_L + GDT_DESC_G_4K) >> 16)
#define GDT_ATTR_HIGH_STACK ((GDT_DESC_AVL + GDT_DESC_D_32 + GDT_DESC_L + GDT_DESC_G_B) >> 16)

#define TSS_SELECTOR (0x5 << 3) + TI_GDT + RPL0

struct desc{
    uint16_t limit_low;
    uint16_t base_address_low;
    uint8_t base_address_middle;
    uint8_t attr_low;
    uint8_t limit_high_attr_high;
    uint8_t base_address_high;
};



#endif