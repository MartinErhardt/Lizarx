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
/*
char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}*/