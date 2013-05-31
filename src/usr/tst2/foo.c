#include"main.h"
int i=2;

void foo2(void);
char hello_shared[]="hello shared\n";
void foo(void)
{
	uprintf(itoa(i,16));
	uprintf(&hello_shared[0]);
	foo2();
}

void foo2(void)
{
	char hello_shared2[]="hello shared 2\n";
	uprintf(&hello_shared2[0]);
}
