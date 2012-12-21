#ifndef VRAM_H
#define VRAM_H

#include <stdint.h>
// Aufschlüsselung des Attribut-Bytes vom Text Modus
typedef struct 
{
    uint8_t foreground : 3;
    uint8_t bold : 1;
    uint8_t background : 3;
    uint8_t blink : 1;
    
}__attribute__((packed)) atrbyt;
// Ein Zeichen
typedef struct{
  
    uint8_t literal;
    atrbyt font;
    
}__attribute__((packed)) figure;

void kprintn(unsigned long x, int base, atrbyt font);
void clrscr(void);
void kputs(const char* s,atrbyt font);
void kput(uint8_t chr, atrbyt font);

uint8_t getcurs();
void setcurs(uint8_t xp,uint8_t yp);

void rmvcurs();
void drawcurs();
#endif
