#ifndef STRING_H
#define STRING_H

#include<stdint.h>

#define NULL ((void*) 0)

void *memcpy(void *dst, const void *src, size_t len);
void *memmove ( void *dst, const void *src, size_t len );
void *memset ( void *ptr, int val, size_t len );

#endif