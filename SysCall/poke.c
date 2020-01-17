#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define FLAG_0 (1 << 0)
#define FLAG_1 (1 << 1)
#define FLAG_2 (1 << 2)
#define FLAG_3 (1 << 3)

char g = 'g';

void decode(unsigned long a);

int main(int arg, char** argv) 
{
	unsigned long PAGE_SIZE = 0, TASK_SIZE = 0, flags = 0, validAddr =0,
		      invalidAddr =0;

	PAGE_SIZE = sysconf(_SC_PAGESIZE);
		if(sizeof(void*) == sizeof (int)) 
			TASK_SIZE = 0xc0000000UL;
		else 
			TASK_SIZE = (1UL << 47) - PAGE_SIZE;
	
	printf("Page Size: %lu\nTask Size: %lu\n\n",PAGE_SIZE,TASK_SIZE);

	// Global variable poke
	printf("Global Variable: Char-\'%c\'\n",g);
	for ( ; (unsigned long) g < TASK_SIZE ;g += PAGE_SIZE*1024) 
	{
		flags = syscall(436,(void*)&g);
		if ((flags & FLAG_0) > 0) {
			validAddr++;
		} else { invalidAddr++; }
		if (flags != 0) 
			break;
	}
	printf("Roughly %lu of %lu addresses allocated in page.\n",validAddr,validAddr+invalidAddr);
	decode(flags);
	flags = 0;
	validAddr = invalidAddr = 0;

	// Local variable poke
	int i = 315135;
	printf("Local Variable Poke: Int-%d\n",i);
	for ( ; (unsigned long) i < TASK_SIZE ;i += PAGE_SIZE*1024) 
	{
		flags = syscall(436,(void*)&i);
		if ((flags & FLAG_0) > 0) {
			validAddr++;
		} else { invalidAddr++; }

		if (flags != 0) 
			break;
	}
	printf("Roughly %lu of %lu addresses allocated in page.\n",validAddr,validAddr+invalidAddr);
	decode(flags);
	flags = 0;
	validAddr = invalidAddr = 0;

	// Malloc-ed buffer poke
	char *buffer = (char*) malloc(sizeof(char));
	printf("Malloc-ed Buffer Poke\n");
	for ( ; (unsigned long) buffer < TASK_SIZE ;buffer += PAGE_SIZE*1024) 
	{
		flags = syscall(436,(void*)&buffer);
		if ((flags & FLAG_0) > 0) {
			validAddr++;
		} else { invalidAddr++; }
		if (flags != 0) 
			break;
	}
	printf("Roughly %lu of %lu addresses allocated in page.\n",validAddr,validAddr+invalidAddr);
	decode(flags);
	flags = 0;
	validAddr = invalidAddr = 0;

	// Function poke
	unsigned long function = (unsigned long) &main;
	printf("Function Poke\n");
	for ( ; function < TASK_SIZE ;function += PAGE_SIZE*1024) 
	{
		flags = syscall(436,(void*)function);
		if ((flags & FLAG_0) > 0) {
			validAddr++;
		} else { invalidAddr++; }
		if (flags != 0) 
			break;
	}
	printf("Roughly %lu of %lu addresses allocated in page.\n",validAddr,validAddr+invalidAddr);
	decode(flags);
	flags = 0;
	validAddr = invalidAddr = 0;

	// Null poke
	void * p = 0;
	printf("Null Address Poke: %p\n",p);
	for ( ; (unsigned long) p < TASK_SIZE ;p += PAGE_SIZE*1024) 
	{
		flags = syscall(436,(void*)&p);
		if ((flags & FLAG_0) > 0) {
			validAddr++;
		} else { invalidAddr++; }
		if (flags != 0) 
			break;
	}
	printf("Roughly %lu of %lu addresses allocated in page.\n",validAddr,validAddr+invalidAddr);
	decode(flags);

	return 0;
}

void decode(unsigned long a) 
{
	if (a == 0) {
		printf("Address not found");
		return;
	}

	printf("P: ");
	if ((a & FLAG_0) > 0) {
		printf("Yes\n");
	} else {
		printf("No\n");
	}

	printf("R: ");
	if ((a & FLAG_1) > 0) {
		printf("Yes\n");
	} else {
		printf("No\n");
	}
	
	printf("W: ");
	if ((a & FLAG_2) > 0) {
		printf("Yes\n");
	} else {
		printf("No\n");
	}
	
	printf("E: ");
	if ((a & FLAG_3) > 0) {
		printf("Yes\n");
	} else {
		printf("No\n");
	}
	printf("\n");

}
