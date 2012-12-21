#ifndef STRING_H
#define STRING_H
// size 
typedef unsigned int size_t;

// Signed pointer-sized integer 
typedef long intptr_t;
// Unsigned pointer-sized integer
typedef unsigned long uintptr_t;

void *memcpy(void *dst, const void *src, size_t len);
void *memmove ( void *dst, const void *src, size_t len );
void *memset ( void *ptr, int val, size_t len );

#endif