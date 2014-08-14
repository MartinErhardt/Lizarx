#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

//#include <_ansi.h>
#include <errno.h>

// --- Process Control ---

int
_exit(int val){
  exit(val);
  return (-1);
}

int
execve(char *name, char **argv, char **env) {
	errno = ENOMEM;
	return -1;
}

/*
 * getpid -- only one process, so just return 1.
 */
#define __MYPID 1
int
getpid()
{
  return __MYPID;
}


int 
fork(void) {
	errno = ENOTSUP;
	return -1;
}


/*
 * kill -- go out via exit...
 */
int
kill(pid, sig)
     int pid;
     int sig;
{
  if(pid == __MYPID)
    _exit(sig);


	errno = EINVAL;
  return -1;
}

int
wait(int *status) {
	errno = ECHILD;
	return -1;
}

// --- I/O ---

/*
 * isatty -- returns 1 if connected to a terminal device,
 *           returns 0 if not. Since we're hooked up to a
 *           serial port, we'll say yes and return a 1.
 */
int
isatty(fd)
     int fd;
{
  return (1);
}


int
close(int file) {
	return -1;
}

int
link(char *old, char *new) {
	errno = EMLINK;
	return -1;
}

int
lseek(int file, int ptr, int dir) {
	return 0;
}

int
open(const char *name, int flags, ...) {
	return -1;
}

int
read(int file, char *ptr, int len) {
	// XXX: keyboard support

	return 0;
}

int 
fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int
stat(const char *file, struct stat *st){
	st->st_mode = S_IFCHR;
	return 0;
}

int
unlink(char *name) {
	errno = ENOENT;
	return -1;
}


int
write(int file, char *ptr, int len) {
	return -1;
}

// --- Memory ---

/* _end is set in the linker command file */
extern caddr_t _end;

#define PAGE_SIZE 4096ULL
#define PAGE_MASK 0xFFFFF000ULL
unsigned long initHeap();
unsigned long initHeap()
{
	unsigned long new_space=0x0;
	asm volatile( "mov $4096, %edx");
	asm volatile( "mov $10, %eax");
	asm volatile ("int $0x30");
	asm volatile("nop" : "=d" (new_space) );
	return new_space;
}

/*
 * sbrk -- changes heap size size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */
caddr_t sbrk(int nbytes)
{
	static unsigned long heap_ptr = 0;
	caddr_t base;
	int temp;
	if(heap_ptr == 0)
	{
		heap_ptr = initHeap();
	}
	base = (caddr_t)heap_ptr;
	if(nbytes < 0)
	{
		heap_ptr -= nbytes;
		return base;
	}
	if( (heap_ptr & ~PAGE_MASK) != 0ULL)
	{
		temp = (PAGE_SIZE - (heap_ptr & ~PAGE_MASK));
		if( nbytes < temp )
		{
			heap_ptr += nbytes;
			nbytes = 0;
		}
		else
		{
			heap_ptr += temp;
			nbytes -= temp;
		}
	}
	while(nbytes > PAGE_SIZE)
	{
		nbytes -= (int) PAGE_SIZE;
		heap_ptr = heap_ptr + PAGE_SIZE;
	}
	if( nbytes > 0)
	{
		heap_ptr += nbytes;
	}
	return base;
}


// --- Other ---
 int gettimeofday(struct timeval *p, void *z){
	 return -1;
 }
