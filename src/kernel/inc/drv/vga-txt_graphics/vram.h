#ifndef VRAM_H
#define VRAM_H

#include <stdint.h>
#include <stdbool.h>

// Aufschlüsselung des Attribut-Bytes vom Text Modus
typedef struct 
{
    uint8_t frgrndnbld : 4;
    uint8_t bckgrndnblnk : 4;
    
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

uint8_t getcurx();
uint8_t getcury();

void setcurs(uint8_t xp,uint8_t yp);

void rmvcurs();
void drawcurs();
#endif
