Compilation:
The open, fopen, and mmap files can all be compiled with the Makefile. 
The default 'make' command will make the open executable, 'make fopen' will compile the fopen executable, 
and the 'make mmap' will compile the mmap executable. However, it is possible to compile all of the executables 
at once using the 'make all' command. They can be cleaned with either clean1, clean2, clean 3, or cleanall

Execution and Process:
1.The open executable is ran via the command line './open ${filename}'
It uses the open function to open the file then reads line by line into a buffer. 
The buffer then parses each character, looking for a alphabetical characters and flipping their case. 
It does so via their ascii code and then either adds or subtracts the required amount to change the case. 
It then writes this buffer into an output file called open_output.txt. This is the only of the three executables that 
produces an output outside of the original file.

2. The fopen executable is ran via the command line './fopen ${filename}'
It uses the fopen function to open the file. It then reads the file descriptor stat structure to find the full size of the file. Once the size is known, it creates a buffer the exact size it needs and parses the data into the buffer in the same manner described in the open executable. It then uses freopen to reopen the original text file and copies the buffered data back into the original file.

3. The mmap executable is ran via the command line './mmap ${filename}'
It uses the mmap function to map the data of the file to a new virtual address. It creates the exact size needed in the same manner that the size was found in the fopen file. It then parses through the information in the exact same manner as both previous executables. Finally, it uses the msync function to sync the original file back with the allocated memory.

