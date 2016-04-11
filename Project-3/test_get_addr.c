/*		*	*	*	*	*	*	*	*	*	*	*	*
*
*		Group Names:		Mark Harman	
*							Wesley Springer
*							Chandrakanth Reddy
*		Assignment:			Project 3 Part I
*		Date:				4/7/16
*
*	*	*	*	*	*	*	*	*	*	*	*	*	*/

#include <unistd.h>			//for testing sys_call
#include <stdio.h>			//for standard I/O
#include <stdlib.h>			//for standard Library

#define __NR_sys_get_addr 359

int main (int argc, char *argv[])
{
	int pid;
	char *ch;
	unsigned long virtual_address;
	unsigned long physical_address;
	unsigned long pfn;

	pid = atoi(argv[1]);
	virtual_address = strtoul(argv[2], &ch, 16);
	pfn = syscall(__NR_get_addr, pid, virtual_address, 0);
	physical_address = syscall(__NR_get_addr, pid, virtual_address, 1);

	if (physical_address == -1)
		printf("That page is not available.\n");
	else 
	{
		printf("Physical Address: 0x%lX\n", physical_address);
		printf("Swap ID: %ld\n", pfn);
	}

	return 0;
}
