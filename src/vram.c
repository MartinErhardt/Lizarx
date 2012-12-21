#include <stdint.h>
#include <vram.h>
#include <ioport.h>
#include <string.h>

// Pointer zum VRAM
static uint16_t* video = (uint16_t*) 0xB8000;
static uint8_t curx = 0;
static uint8_t cury = 0;

static void newline();
//Clear Screen function

void clrscr(void)
{
    int i;
    for (i = 0; i < 2 * 25 * 80; i++) {
        video[i] = 0;
    }
    curx=cury=0;
}

// put stream in memory and increment X
void kputs(const char* s,atrbyt font)
{
    while (*s) {
        kput(*s++,font);
    }
}

void kput(uint8_t chr, atrbyt font)
{
  figure fig={chr, font};
  
  if(cury>=25){// Test
    return;
  }
  if((curx>=80)||(chr =='\n')){// Test
    newline();
    return;
  } 
  // berechnen der Adresse
  figure* adr = (uint16_t*) (video+(cury * 80) + curx);  // eine Multiplikation mit 2 darf hier nicht erfolgen, da off vom type uint16_tist
  
  // setzen des zeichens
  memcpy(adr,&fig,2);
  //*adr = fig;
  curx++;
}
uint8_t getcurs(){
  uint8_t pos[]={curx,cury};
  return pos;
}
void setcurs(uint8_t xp,uint8_t yp){
  curx = xp;
  cury = yp;
}
void drawcurs(){
  uint32_t tmp;
  tmp= cury*80+curx+1;
  outb(0x3D4,14);
  outb(0x3D5,tmp >> 8);
  outb(0x3D4,15);
  outb(0x3D5,tmp);
}
void rmvcurs()
{
  outb(0x3D4,14);
  outb(0x3D5,0x07);
  outb(0x3D4,15);
  outb(0x3D5,0xD0);
}
 
static void newline(){
    cury++;
    curx=0;
}
