#include"main.h"

void foo(void)
{
	char hello_shared[]="hello shared";
	uprintf(&hello_shared[0]);
}
