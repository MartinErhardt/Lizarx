#include<stdlib.h>
/**
 * converts a long to a char
 * @return ptr to converted int
 **/
char * itoa(int x, int radix) {
    char buf[65];
    const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* p=0x0;
    
    if (radix > 36) {// radix mustn't be larger than 36 for security(overflow)
        return 0x0;
    }

    p = buf + 64;
    *p = '\0';
    do {
        *--p = digits[x % radix];
        x /= radix;
    } while (x);
    return p;
}