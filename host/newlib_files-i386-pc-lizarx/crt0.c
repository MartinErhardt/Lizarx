extern int main(int, char**); //int argc, char **argv, char **environ);
void _start();

void _start()
{
	int argc=0; 
	char** argv=(void*)0x0;
	start3(0,(char**)0x0);
	while(1);
	
}
int start3(int argc, char** argv){
	unsigned long long *origin, *target;
	int i;
	
	for(i = 0; i < argc; i++)
	{
		origin = ((unsigned long long*)argv) + ((i * 2) + 1);
		target = ((unsigned long long*)argv) + i;
		*target = *origin;
	}
	i = main(argc, argv);
	asm volatile ("int $0x30"::"a" (12) );
	while(1);
	return i;
}
