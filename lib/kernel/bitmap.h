#ifndef __LIB_KERNEL_BITMAP_H
#define __LIB_KERNEL_BITMAP_H
#include "stdint.h"

#define BITMAP_MASK 1

struct bitmap{
    uint32_t map_bytes_len;
    uint8_t * bits;
};
void bitmap_init(struct bitmap * map);
uint32_t bitmap_scan_test(struct bitmap * map, uint32_t bit_idx);
uint32_t bitmap_scan(struct bitmap * map, uint32_t n);
void bitmap_set(struct bitmap * map, uint32_t bit_idx, uint8_t value);

#endif