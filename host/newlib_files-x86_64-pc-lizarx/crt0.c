#if defined(__cplusplus)
extern "C" 
{
	int main(int, char**); //int argc, char **argv, char **environ);
	void _start()
}
#else
extern int main(int, char**); //int argc, char **argv, char **environ);
void _start();
#endif
#define SYSCALL(N)	asm volatile ("syscall"::"a" (N) );
void _start()
{
	int argc=0; 
	char** argv=(void*)0x0;
	main(argc, argv);
	SYSCALL(12)
}
