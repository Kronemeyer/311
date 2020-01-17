#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int arg, char ** argv) {

    size_t size;
    struct stat sb;
    char *addr;
    char* outfile = "fopen_output.txt";

    int fd = open(argv[1],O_RDWR);
    fstat(fd,&sb);
    size = sb.st_size;

    addr = (char*) mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    for (int i = 0;i < size; i++) {
        // Lower Case
        if (addr[i] >= 97 && addr[i] <= 122) {
            addr[i] -= 32;
        }
        // Upper Case
        else if (addr[i] >= 65 && addr[i] <= 90) {
            addr[i] += 32;
        }
    }
    
    msync(addr,size,MS_SYNC);
    munmap(addr,size);
    close(fd);
    printf("Character cases of file have been reversed within the original document.\n");
    return 1;
}