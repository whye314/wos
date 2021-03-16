#include "bitmap.h"
#include "stdint.h"
#include "string.h"
#include "print.h"
#include "interrupt.h"
#include "debug.h"

void bitmap_init(struct bitmap * map){
    memset(map->bits, 0, map->map_bytes_len);
}

uint32_t bitmap_scan_test(struct bitmap * map, uint32_t bit_idx){
    ASSERT(bit_idx < map->map_bytes_len * 8);
    uint32_t byte_idx = bit_idx / 8;
    uint32_t bit_offset = bit_idx % 8;
    return map->bits[byte_idx] & (BITMAP_MASK << bit_offset) ? 1 : 0;
}

uint32_t bitmap_scan(struct bitmap * map, uint32_t n){
    uint32_t start_free_byte_idx = 0;
    while((map->bits[start_free_byte_idx]) == 0xff && (start_free_byte_idx < map->map_bytes_len)) start_free_byte_idx++;
    //if(start_free_byte_idx >= map->map_bytes_len) return -1;
    uint32_t start_free_bit_idx = start_free_byte_idx * 8;
    uint32_t r = start_free_bit_idx, count = 0;
    while((count < n) && (start_free_bit_idx < map->map_bytes_len * 8)){
        if(!bitmap_scan_test(map, start_free_bit_idx++)){
            count++;
        }
        else{
            count = 0;
        }
    }
    return count == n ? (start_free_bit_idx - n) : -1;

}

void bitmap_set(struct bitmap * map, uint32_t bit_idx, uint8_t value){
    ASSERT((value == 0 || value == 1) && (bit_idx < map->map_bytes_len * 8));
    uint32_t byte_idx = bit_idx / 8;
    uint32_t bit_offset = bit_idx % 8;
    if(value){
        map->bits[byte_idx] |= (BITMAP_MASK << bit_offset);
    }
    else{
        map->bits[byte_idx] &= ~(BITMAP_MASK << bit_offset);
    }
}