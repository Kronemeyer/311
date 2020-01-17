#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

int main(int arg) {
	unsigned long hello = syscall(437);
	return 0;
}
