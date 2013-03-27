#ifndef VHEAP_H
#define VHEAP_H

#include<stdint.h>

void vheap_init();
void* kmalloc(size_t size);
void kfree(void* ptr);
void* krealloc();

#endif