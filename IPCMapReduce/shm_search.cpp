#include <algorithm>
#include <fcntl.h>
#include <fstream>
using std::ifstream;
#include <iostream>
using std::cout;
using std::endl;
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
using std::string;
using std::swap;
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
using std::vector;
#include <chrono>
using namespace std::chrono;

#define SNAME "/shmsem"
#define NAME "OS"
#define NUM_THREADS 4


struct pthread_args
{
	int id;
	string sword;
	vector<string> vect;
};

void *DoWork(void *arguments);

int main(int argc, char *argv[])
{
	auto start = high_resolution_clock::now();
	pid_t pid, wpid;
	sem_t *sem;

	sem = sem_open(SNAME, O_CREAT, 0666, 0);
	pid = fork();

	/*--------------------PARENT PROCESS----------------------------*/
	if (pid > 0)
	{
		int status, fd, shm_fd;
		struct stat sb;
		char *addr;

		fd = open(argv[1], O_RDWR);

		if (fd == -1)
		{
			perror("");
			exit(1);
		}
		if (fstat(fd, &sb) == -1)
		{
			perror("");
			exit(1);
		}
		shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
		ftruncate(shm_fd, sb.st_size);
		addr = (char *)mmap(0, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if (addr == MAP_FAILED)
		{
			perror("");
			exit(1);
		}
		close(fd);

		ifstream file;
		string line, text;
		file.open(argv[1]);
		while (getline(file, line, '.'))
		{
			text += line + ".";
		}
		const char *send = text.c_str();
		memcpy(addr, send, sb.st_size);
		sem_post(sem);

		do
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		char returnc[sb.st_size];
		strncpy(returnc,addr,sb.st_size);
		string parse = returnc;
		string::iterator st;
		size_t index =1;
		cout << index << ") ";
		for(st = parse.begin();st < parse.end();st++){
			cout << *st;
			if ((*st == '.' || *st == ';' || *st == '?' || *st == '!') && (st+1 != parse.end()-1)){
				cout << endl;
				cout << ++index << ") ";
				st++;
			}
		}
		shm_unlink(NAME);
		sem_unlink(SNAME);
		sem_close(sem);
		sem_destroy(sem);
		wait(NULL);
		auto stop = high_resolution_clock::now();
		auto end = duration_cast<microseconds>(stop-start);
		cout << "Subproject 2 run time: " << end.count() << endl;
	}

	/*---------------------CHILD PROCESS----------------------------*/
	if (pid == 0)
	{
		struct stat sb;

		// ONLY USED TO GET FILE SIZE.I COULDNT FIGURE OUT HOW TO SEND IT BEFORE MAKING THE SHARED MEMORY
		int fd = open(argv[1], O_RDONLY);
		fstat(fd, &sb);
		int size = sb.st_size;
		close(fd);

		int shm_fd, rc;
		char *addr;
		struct pthread_args args;
		args.sword = argv[2];

		sem_t *sem = sem_open(SNAME, 0);
		sem_t *sem1 = sem_open("/child", O_CREAT, 0666, 0);
		sem_wait(sem);

		shm_fd = shm_open(NAME, O_RDWR, 0666);
		addr = (char *)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		char file[size + 1];
		strncpy(file, addr, size);
		strcat(file, "\0");
		string sentence;

		// read from the shared memory and parse characters accordingly
		for (int i = 0; i < size; i++)
		{
			if (isalpha(file[i]) || file[i] == ' ' || file[i] == '\'')
			{
				sentence.append(1, file[i]);
			}
			else if (file[i] == '\"' && (isalpha(file[i + 1]) || file[i + 1] == ' '))
			{
				sentence.append(1, file[i]);
			}
			else if ((file[i] == '.' || file[i] == '!' || file[i] == '?' || file[i] == ';') &&
					 (file[i + 1] == '\n' || file[i + 1] == '\"' || file[i + 1] == ' '))
			{
				sentence.append(1, file[i++]);
				sentence.append(1, file[i]);
				if (file[i] == '\"')
				{
					sentence.append(1, file[++i]);
				}
				sentence.append(1, '\0');
				args.vect.push_back(sentence);
				sentence.erase();
			}
			else if (file[i] == '\n')
			{
				sentence. append(1,' ');
			}
			else if (file[i] == '\0')
			{
				sentence.append(1, file[i]);
				args.vect.push_back(sentence);
				sentence.erase();
			}
		}

		pthread_t threads[NUM_THREADS];
		pthread_attr_t att;
		void *status;
		int i;

		pthread_attr_init(&att);
		pthread_attr_setdetachstate(&att, PTHREAD_CREATE_JOINABLE);

		for (i = 0; i < NUM_THREADS; i++)
		{
			args.id = i;
			rc = pthread_create(&threads[i], &att, DoWork, (void *)&args);
			if (rc)
			{
				perror("");
				exit(1);
			}
			sem_wait(sem1);
		}
		pthread_attr_destroy(&att);
		for (i = 0; i < NUM_THREADS; i++)
		{
			rc = pthread_join(threads[i], &status);
			if (rc)
			{
				perror("");
				exit(1);
			}
		}
		/* iterate through the vector that now holds only 
		elements with search word or "Delete\0" and delete unneeded */
		vector<string>::iterator it = args.vect.begin();
		while (it != args.vect.end())
		{
			if (it->find("Delete\0") != string::npos)
			{
				it = args.vect.erase(it);
			}
			else
			{
				++it;
			}
		}
		cout << args.vect.size() << endl;
		// format all entries in the vector to ensure proper sorting
		for (i = 0; i < args.vect.size(); i++)
		{	
			while (args.vect.at(i).front() == ' ')
			{
				args.vect.at(i).erase(0, 1);
			}
			if (args.vect.at(i).front() == '\"')
			{
				swap(args.vect.at(i).at(0), args.vect.at(i).at(1));
			}
			if (islower(args.vect.at(i).front()))
			{
				args.vect.at(i).front() = toupper(args.vect.at(i).front());
			}
		}
		sort(args.vect.begin(), args.vect.end());
		// reverse any swaps made and add a new line to the end of each string
		for (i = 0; i < args.vect.size(); i++)
		{	
			if (args.vect.at(i).at(1) == '\"')
			{
				swap(args.vect.at(i).at(0), args.vect.at(i).at(1));
			}
			// put a new line character at the end of each line for easy reading to shared memory
			if (args.vect.at(i).back() != '\n') 
			{
				args.vect.at(i).push_back('\n');
			}
		}
		sentence = "";
		// reset the shared memory to ensure no residual data and write to memory
		memset(addr,0,size);
		for (i = 0; i < args.vect.size(); i++)
		{
			sentence =  args.vect.at(i);
			const char *send = (sentence.c_str());
			memcpy(addr,send,strlen(send));
			addr += strlen(send);
		}
		// delete sem1, threads, and unlink memory
		sem_close(sem1);
		sem_destroy(sem1);
		pthread_exit(NULL);
		shm_unlink(NAME);
	}
	// ERROR IN FORK
	if (pid < 0)
	{
		cout << "Error in fork." << endl;
		exit(1);
	}
}

/* Assigns the pthread_args struct to each thread which includes
	the vector of all sentences, search word to search for,
	and thread id, used to divide the vector*/
void *DoWork(void *arguments)
{
	struct pthread_args *args = (struct pthread_args *)arguments;
	int tid = args->id;
	string sword = args->sword;
	sem_t *sem1 = sem_open("/child", 0);
	sem_post(sem1);
	size_t found, slen;
	slen = sword.length();
	// divide the vector between threads and search for words.
	for (int i = 0; i < args->vect.size(); i++)
	{
		if ((i % NUM_THREADS) == tid)
		{
			string search = args->vect[i];
			if ((found = search.find(sword)) != string::npos)
			{
				found += (slen);
				if (!isalpha(search.at(found)))
				{ // if next letter isnt a letter youre done
				}
				else
				{
					while ((found <= (search.length() - slen)) && found != string::npos)
					{
						found = search.find(sword, found);
						if (found != string::npos)
						{
							found += slen;
							if (!isalpha(search.at(found)))
							{
								break;
							}
						}
						else
						{
							args->vect.at(i) = "Delete\0";
						}
					}
					found = 0;
				}
			}
			else
			{
				args->vect.at(i) = "Delete\0";
			}
		}
	}
	pthread_exit(NULL);
}