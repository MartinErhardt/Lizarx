#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <drv/vga-txt_graphics/vram.h>
#include <drv/io/ioport.h>

#define VIDEO_X 80
#define VIDEO_Y 25
/*
  NOTE: I decided to implement the VGA-txt-graphics driver into the Kernel;
	This goes with an increase of Performance. but also with an increase of Syscalls and complexity
 */
// Pointer zum VRAM
static uint16_t* video = (uint16_t*) 0xB8000;
static size_t vram_size =sizeof(figure)*VIDEO_Y*VIDEO_X;
static uint8_t curx = 0;
static uint8_t cury = 0;
// printed chars
static uint32_t kprintf_res = 0;
static bool scr_enb = TRUE;

static void scroll(uint8_t by);

static void newline();
//Clear Screen function

void clrscr(uint8_t forgcol, uint8_t backcol)
{
    uint16_t fig=0xffff & (forgcol|(backcol>>4));
    memset(video,(uint32_t) fig | (fig<<16),sizeof(figure)*VIDEO_X*VIDEO_Y);
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
  if((scr_enb==TRUE)&&(cury==VIDEO_Y)){
      scroll(1);
  }
  if((curx>=VIDEO_X)||(chr =='\n')){// Test
    newline();
    return;
  } 
  if(chr == '\t'){
      curx = curx - curx%8 +8;
      return;
  }
  // berechnen der Adresse
  uint16_t* adr = (uint16_t*) (video+(cury * VIDEO_X) + curx);  // eine Multiplikation mit 2 darf hier nicht erfolgen, da off vom type uint16_t ist
  
  // setzen des zeichens
  memmove(adr,&fig,sizeof(fig));
  //*adr = fig;
  curx++;
}

uint8_t getcurx(){
  return curx;
}
uint8_t getcury(){
  return cury;
}

void setcurs(uint8_t xp,uint8_t yp){
  curx = xp;
  cury = yp;
}

void drawcurs(){
  uint16_t tmp= (cury*VIDEO_X+curx)-1;
  outb(0x3d4,14);
  outb(0x3d5,tmp >> 8);
  outb(0x3d4,15);
  outb(0x3d5,tmp);
}
void rmvcurs()
{
  outb(0x3d4,14);
  outb(0x3d5,0x07);
  outb(0x3d4,15);
  outb(0x3d5,0xd0);
}
 
static void newline(){
    cury++;
    curx=0;
}

void kprintn_scr(unsigned long x, int base, atrbyt font)
{
    kputs(itoa(x,base),font);
}

//Kernel printcolor prints a colored string

uint32_t kprintfcol_scr(uint8_t forgcol, uint8_t backcol, const char* fmt, ...){
    va_list ap;
    atrbyt font={backcol, forgcol};
    //CALL_VA_FUNC(kprintfstrcol,font,fmt);
    
    va_start(ap,fmt);
    kprintfstrcol_scr(font,fmt,ap);
    va_end(ap);

    return kprintf_res;
}

//Kernel print string colored prints a colored string

uint32_t kprintfstrcol_scr(atrbyt font, const char* fmt, va_list appar){
    const char* s;
    uint32_t n;

    kprintf_res = 0;
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's':
                    s = va_arg(appar, char*);
                    kputs(s,font);
                    break;
                case 'd':
                case 'u':
                    n = va_arg(appar, unsigned long int);
                    kprintn_scr(n, 10,font);
                    break;
                case 'x':
                case 'p':
                    n = va_arg(appar, unsigned long int);
                    kprintn_scr(n, 16,font);
                    break;
                case '%':
                    kput('%',font);
                    break;
                case '\0':
                    goto out;
                default:
                    kput('%',font);
                    kput(*fmt,font);
                    break;
            }
        } else {
            kput(*fmt,font);
        }
	
        fmt++;
    }

out:
    va_end(appar);
    drawcurs();
    return kprintf_res;
}
//FIXME puffer awayscrolled chars to scroll-back later
static void scroll(uint8_t by){
    size_t off = sizeof(figure)*VIDEO_X*by;
    size_t size_of_copy=vram_size-off;
    memmove(video,(uint32_t*)((uintptr_t)video+off),size_of_copy);
    memset((uint32_t*)((uintptr_t)video+size_of_copy),0x0,off);//FIXME 0x0 can be something else than the backgroundcol
    cury=cury-by;
}