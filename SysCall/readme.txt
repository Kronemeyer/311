DESIGN:
My design for Project 3 was to add my system calls to the already made sys.c file. I did this by using the SYSCALL_DEFINE macro, specifying if the syscall had 0 or 1 parameters to be passed (hello = 0, poke = 1).

My system call creates a task_struct and from that I traverse through the linked list of VMA's via the mm_struct using vm_next. The poke function will then look for a specific address and return either an unsigned long variable named temp or 0.

First, I retrieve the page offset for the page currently being accessed and check the first bit to see whether the page is currently in hard disk. I then set the first bit of the temp variable accordingly. Then, when the address is found, i set the 2nd, 3rd, and 4th bit of the temp variable according to the 1st, 2nd, and 3rd bits of the vm_flags for that address. The temp variable will then accurately describe whether the page is present and whether the address is read, write, or execute protected. If the address is not found I simply return a 0.

In the userspace, I then traverse addresses based on Page Size to ensure I hit every page possible within the Page Table. Once the address has been found I then decode the 4 bits of the returned unsigned long to determine address space priviledges.

I will admit, I do not know how to accurately describe how many address are currently allocated vs how many are not. I thought it would have something to do with the page offset and whether the page was in memory or not. However, I am sure that is very wrong.

TO COMPILE AND USE
To compile my userspace code, simply run the Make file provided with make. Its default make is to create the poke executable.
To compile the system call apply the kernel patch to your own patch

To use the code, simply run ./poke within the directory that contains the poke executable. To change the pokable address one would have to alter the poke.c file to include a new variable address or physical address and then recompile using the Makefile.

REPORT:
1. What API's are used to allocate memory in Linux user space programs, and when to use which.
	- malloc()/calloc()/realloc() - create memory on the heap
		malloc() should be used when a simple block of memory is needed. The block is uninitilialized.
		calloc() should be used when contiguous allocation of memory is important. It can also be passed two arguments, the number of blocks to create and the size of each block. This allows the user to create a certain size of memory that can hold a particular amount of elements all n size. It is mainly used for storage of dynamic arrays.
		realloc() should be used when you want to change the size of a memory block on the heap. This will allow you to change the size of a previously allocated memory space.
	- mmap() - Anonymous memory
		mmap() should be used when you wish to store a file from disk into memory. It will not create fresh memory but instead copy the file to new memory.


2. When an address p is dereferenced, you may encounter SIGSEGV. Describe how the system recognizes p is an invalid address and triggers a SIGSEGV.
	When an address is looked up, the virtual address will first be concurrently searched in the Translation Lookaside Buffer (TLB) using the virtual address Page #.
	If the address is within the TLB it will immediately go to the address and read the vm_flags. However, if the address no longer resides in the TLBthe machine will then look up the virtual address via the Page Table Lookup, this will perform a page table walk and search the VMA's until it realizes that the address requested is no longer accessable to whatever is calling it, it will cause an interupt and a SIGSEGV will be issued to indicate a Segmentation fault has occured.
