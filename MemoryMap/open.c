#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SIZE 4096

int main(int arg, char ** argv) {
 
    int ifd, tfd, bread, fileSize=0;
    char buffer[SIZE];
    char* infile = argv[1];

    if ((ifd = open(infile,O_RDONLY)) == -1) {
        printf("Cannot open %s.",infile);
        exit(-1);
    }
    if ((tfd = open("open_output.txt",O_RDWR|O_CREAT,0666)) == -1) {
        printf("Output failed");
        exit(-1);
    }

    int count,i;
    while ((bread = read(ifd,&buffer,SIZE)) !=0 ) {
        for(i=0;i<SIZE;i++) {
            // Lower Case
            if (buffer[i] >= 97 && buffer[i] <= 122) {
                buffer[i] -= 32;
            }
            // Upper Case
            else if (buffer[i] >= 65 && buffer[i] <= 90) {
                buffer[i] += 32;
            // End of File
            } else if (buffer[i] == '\0') {
                break;
            }
        }
        // If end of file smaller than SIZE
        if (i<SIZE) {
            write(tfd,&buffer,i);
        // Else just write SIZE
        } else {
            write(tfd,&buffer,SIZE);
        }
        memset(buffer,0,SIZE);
    }
    close(tfd);
    close(ifd);

    printf("Character cases of file have been reversed and put into open_output.txt.\n");
    return 1;
}