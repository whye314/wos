#include "list.h"
#include "string.h"
#include "interrupt.h"
#include "stdint.h"

void list_init(struct list * list){
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->head.prev = NULL;
    list->tail.next = NULL;
}

void list_insert(struct list_elem * before, struct list_elem * node){
    enum int_status old_status = int_disable();
    node->prev = before->prev;
    node->next = before;
    before->prev->next = node;
    before->prev = node;
    int_set_status(old_status);
}

void list_push(struct list * plist, struct list_elem * node){
    list_insert(plist->head.next, node);
}

void list_append(struct list * plist, struct list_elem * node){
    list_insert(&plist->tail, node);
}

void list_remove(struct list_elem * node){
    enum int_status old_status = int_disable();
    node->prev->next = node->next;
    node->next->prev = node->prev;
    int_set_status(old_status);
}

struct list_elem * list_pop(struct list * plist){
    enum int_status old_status = int_disable();
    struct list_elem * node = plist->head.next;
    list_remove(node);
    int_set_status(old_status);
    return node;
    
}

struct list_elem * list_traversal(struct list * plist, function func, int arg){
    enum int_status old_status = int_disable();
    if(list_isempty(plist)) return NULL;
    struct list_elem * node = plist->head.next;
    while(node != &plist->tail){
        if(func(node, arg)) {
            int_set_status(old_status);
            return node;    
        }
        node = node->next;
    }
    int_set_status(old_status);
    return NULL;
}

uint8_t list_find(struct list * plist, struct list_elem * node){
    enum int_status old_status = int_disable();
    struct list_elem * cnode = plist->head.next;
    while(cnode != &plist->tail){
        if(node == cnode){
            int_set_status(old_status);
            return 1;
        }
        cnode = cnode->next;
    }
    int_set_status(old_status);
    return 0;

}

uint8_t list_isempty(struct list * plist){
    return (plist->head.next == &plist->tail)? 1 : 0;
}

uint32_t list_len(struct list * plist){
    enum int_status old_status = int_disable();
    uint32_t len = 0;
    struct list_elem * node = plist->head.next;
    while(node != &plist->tail){
        len ++;
        node = node->next;
    }
    return len;
    int_set_status(old_status);
}