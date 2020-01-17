#include <algorithm>
#include <unistd.h>
#include <vector>
using std::vector;
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <iostream>
using std::endl;
using std::cout;
#include <fstream>
using std::ifstream;
#include <stdio.h>
#include <string.h>
using std::string;
using std::swap;
using std::stoi;
#include <chrono>
using namespace std::chrono;

#define SCLENGTH 4096
#define CLENGTH 4

int main(int argc, char *argv[])
{
	auto start = high_resolution_clock::now();
	pid_t pid, wpid;
	int status, socket[2];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket) == -1)
	{
		perror("Socketspair error");
		exit(1);
	}

	pid = fork();

	/*--------------------PARENT PROCESS----------------------------*/
	if (pid > 0)
	{

		string file_sentence;
		char str[SCLENGTH], bytes[CLENGTH];
		unsigned int amount = 0;

		amount = strlen(argv[2]);
		sprintf(bytes, "%u", amount);
		write(socket[0], bytes, CLENGTH);
		write(socket[0], argv[2], amount);
		memset(bytes, 0, CLENGTH);

		ifstream file(argv[1]);
		if (file.good())
		{
			// while there is still a sentence in the file, get that sentence
			while (getline(file, file_sentence, '.') || getline(file, file_sentence,';') || getline(file,file_sentence,'?') || getline(file,file_sentence,'!'))
			{
				if (file.eof())
					break;
				char sentence[file_sentence.size() + 1];
				strcpy(sentence, file_sentence.c_str());
				strcat(sentence, ".");
				amount = strlen(sentence);
				sprintf(bytes, "%u", amount);
				// send frame to child with length of bits to read
				write(socket[0], bytes, CLENGTH);
				// send actual data to child
				write(socket[0], sentence, amount);
			}
		}
		// shut down the write portion of parents socket to signal the end of transmition.
		// socket still partially open to allow child to write back to parent
		shutdown(socket[0], SHUT_WR);

		// wait for the child to finish
		do 
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		vector<string> vect;
		int toread = 0;
		string sent;

		// read the responses back from the child into a vector to sort
		while (read(socket[0], str, CLENGTH) > 0)
		{
			toread = stoi(str);
			memset(str, 0, SCLENGTH);
			read(socket[0], str, toread);
			sent = str;
			memset(str, 0, SCLENGTH);
			vect.push_back(sent);
		}
		// erase any leading whitespace and then format for correct sorting
		for (int i = 0; i < vect.size(); i++)
		{	
			while (vect.at(i).front() == ' ')
			{
				vect.at(i).erase(0, 1);
			}
			if (vect.at(i).front() == '\"')
			{
				swap(vect.at(i).at(0), vect.at(i).at(1));
			}
			if (islower(vect.at(i).front()))
			{
				vect.at(i).front() = toupper(vect.at(i).front());
			}
		}
		sort(vect.begin(), vect.end());
		// reverse any swaps made and add a new line to the end of each string
		for (int i = 0; i < vect.size(); i++)
		{	
			if (vect.at(i).at(1) == '\"')
			{
				swap(vect.at(i).at(0), vect.at(i).at(1));
			}
			if (vect.at(i).back() != '\n') 
			{
				vect.at(i).push_back('\n');
			}
		}
		for (size_t i = 0; i < vect.size(); ++i)
		{
			cout << vect[i] << endl;
		}
		auto stop = high_resolution_clock::now();
		auto end = duration_cast<microseconds>(stop-start);
		cout << "Subproject 2 run time: " << end.count() << endl;
	}

	/*---------------------CHILD PROCESS----------------------------*/
	if (pid == 0)
	{

		char str[SCLENGTH], bytes[CLENGTH];
		int count = 0, toread = 0;
		unsigned int amount = 0;
		vector<string> vect;
		string sent;

		// read the search word from the socket initially
		read(socket[1], str, CLENGTH);
		toread = stoi(str);
		memset(str, 0, SCLENGTH);
		char word[toread];
		read(socket[1], str, toread);
		string search_word = str;
		size_t slen, found;
		slen = search_word.length();
		cout << "Search word is: " << search_word << endl;
		memset(str, 0, SCLENGTH);

		while (read(socket[1], str, CLENGTH) > 0)
		{
			toread = stoi(str);
			memset(str, 0, SCLENGTH);
			read(socket[1], str, toread);
			sent = str;
			if ((found = sent.find(search_word)) != string::npos)
			{
				found += slen;
				if (!isalpha(sent.at(found))) {
					 // if next letter isnt a letter youre done, send it back
					sprintf(bytes, "%u", toread);
					write(socket[1], bytes, CLENGTH);
					write(socket[1], str, toread);
					count++;
				} else {
					// continue searching from first possible occurance
					while ((found <= (sent.length()-slen)) && found != string::npos) {
						found = sent.find(search_word,found);
						if (found != string::npos) {
							found += slen;
							if(!isalpha(sent.at(found))) {
								//word is here, send it back
								sprintf(bytes, "%u", toread);
								write(socket[1], bytes, CLENGTH);
								write(socket[1], str, toread);
								count++;
							}
						}
					}
					found = 0;
				}
			}		
		memset(str, 0, SCLENGTH);
		}
		printf("Number of matching sentences found: %i\n", count);
		shutdown(socket[1], SHUT_WR);
	}
	// ERROR IN FORK
	if (pid < 0)
	{
		cout << "Error in fork." << endl;
		exit(1);
	}
}
