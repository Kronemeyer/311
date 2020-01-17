default: open.o
	gcc open.c -o open
fopen: fopen.o
	gcc fopen.c -o fopen
mmap: mmap.o
	gcc mmap.c -o mmap
all: open.o fopen.o mmap.o
	gcc open.c -o open
	gcc fopen.c -o fopen
	gcc mmap.c -o mmap
clean1:
	rm open.o open
clean2:
	rm fopen.o fopen
clean3:
	rm mmap.o mmap
cleanall:
	rm *.o open fopen mmap
