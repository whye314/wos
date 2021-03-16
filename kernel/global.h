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

#endif