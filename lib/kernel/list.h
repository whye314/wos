#ifndef __LIB_KERNEL_LIST_H
#define __LIB_KERNEL_LIST_H
#include "stdint.h"

#define offset(struct_type, member) (uint32_t)(&(((struct_type*)0)->member))
#define get_address_from_member(struct_type, member, address) ((struct_type*)(((uint32_t)address) - offset(struct_type, member)))

struct list_elem{
    struct list_elem * prev;
    struct list_elem * next; 
};

struct list{
    struct list_elem head;
    struct list_elem tail;
};

typedef uint8_t function(struct list_elem*, int args);

void list_init(struct list * list);
void list_insert(struct list_elem * before, struct list_elem * node);
void list_push(struct list * plist, struct list_elem * node);
void list_append(struct list * plist, struct list_elem * node);
void list_remove(struct list_elem * node);
struct list_elem * list_pop(struct list * plist);
struct list_elem * list_traversal(struct list * plist, function func, int arg);
uint8_t list_find(struct list * plist, struct list_elem * node);
uint8_t list_isempty(struct list * plist);
uint32_t list_len(struct list * plist);



#endif