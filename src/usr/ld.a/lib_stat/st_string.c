/*   <src-path>/src/kernel/lib/string.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *   Copyright (C) 2013  martin.erhardt98@googlemail.com
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "st_string.h"

void* st_memcpy(void* dest, const void* src, st_size_t n)
{
    unsigned char* d = dest;
    const unsigned char* s = src;
      /*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
    while (n--) {
        *d++ = *s++;
    }

    return dest;
}
/*
 * copy a block of memory, handling overlapping
 * regions correctly.
 */

void * st_memmove(void *dst, const void *src, st_size_t len)
{
      st_size_t i;

      /*
	* If the buffers don't overlap, it doesn't matter what direction
	* we copy in. If they do, it does, so just assume they always do.
	* We don't concern ourselves with the possibility that the region
	* to copy might roll over across the top of memory, because it's
	* not going to happen.
	*
	* If the destination is above the source, we have to copy
	* back to front to avoid overwriting the data we want to
	* copy.
	*
	*      dest:       dddddddd
	*      src:    ssssssss   ^
	*              |   ^  |___|
	*              |___|
	*
	* If the destination is below the source, we have to copy
	* front to back.
	*
	*      dest:   dddddddd
	*      src:    ^   ssssssss
	*              |___|  ^   |
	*                     |___|
	*/

      if ((st_uintptr_t)dst < (st_uintptr_t)src) {
	      /*
		* As author/maintainer of libc, take advantage of the
		* fact that we know memcpy copies forwards.
		*/
	      return st_memcpy(dst, src, len);
      }

      /*
	* Copy by words in the common case. Look in memcpy.c for more
	* information.
	*/

      if ((st_uintptr_t)dst % sizeof(long) == 0 &&
	  (st_uintptr_t)src % sizeof(long) == 0 &&
	  len % sizeof(long) == 0) {

	      long *d = dst;
	      const long *s = src;

	      /*
		* The reason we copy index i-1 and test i>0 is that
		* i is unsigned - so testing i>=0 doesn't work.
		*/

	      for (i=len/sizeof(long); i>0; i--) {
		      d[i-1] = s[i-1];
	      }
      }
      else {
	      char *d = dst;
	      const char *s = src;

	      for (i=len; i>0; i--) {
		      d[i-1] = s[i-1];
	      }
      }

      return dst;
}
void *st_memset(void *ptr, st_uint32_t val, st_size_t len)
{
      st_uint32_t *p = ptr;
      st_size_t i;
      //kprintf("[LIB] I: memset called");
      for (i=0; i<len/4; i++) {
		p[i] = val;
      }
      if(len%4)
      {
		val&= ~(0xffffffff<<(len%4*8));
		p[i+1]=val;
      }

      return ptr;
}
st_size_t st_strlen(const char *s) 
{

    const char *sret = s;

    while (*s) 
    {
        s++;
    }

    return s - sret;
}
int st_strcmp(const char *s1, const char *s2)
{

    while (*s1 && *s1 == *s2) 
    {
        ++s1;
        ++s2;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}