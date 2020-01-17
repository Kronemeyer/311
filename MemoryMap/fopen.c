#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int arg, char ** argv) {
 
    long fileSize;
    char * buffer;
    char* infile = argv[1];

    FILE * fd;

    if ((fd = fopen(infile,"r")) == NULL) {
        printf("Cannot open %s.",infile);
        exit(-1);
    }

    /* 
    *  Get size of original file to create 
    *  exact size of buffer needed
    */ 

    fseek(fd,0,SEEK_END);
    fileSize = ftell(fd);
    rewind(fd);

    // Put all of file in buffer
    buffer = (char*) malloc (sizeof(char)*fileSize);
    fread(buffer,1,fileSize,fd);

    for (int i = 0;i < fileSize; i++) {
        // Lower Case
        if (buffer[i] >= 97 && buffer[i] <= 122) {
            buffer[i] -= 32;
        }
        // Upper Case
        else if (buffer[i] >= 65 && buffer[i] <= 90) {
            buffer[i] += 32;
        }
    }

    // Reopen file as an empty file and write buffer to it
    freopen(infile,"w",fd);
    fwrite(buffer,sizeof(char),fileSize,fd);

    fclose(fd);
    free(buffer);
    printf("Character cases of file have been reversed within the original document.\n");
    return 1;
}