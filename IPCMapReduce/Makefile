CC=g++
CFLAGS=-Wall
NAME=-o
CXXFLAGS=-lrt
PTHREAD=-pthread

default: shm_search.o 
	$(CC) $(PTHREAD) $(CFLAGS) $(NAME) shm_search shm_search.o $(CXXFLAGS)

uds: uds_search.o
	$(CC) $(CFLAGS) $(NAME) uds_search uds_search.o
	
clean:
	@rm *.o uds_search shm_search
