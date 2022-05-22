
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <ctype.h>

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

#include <chrono>
#include <ctime>

using namespace std;

#define MAX 80
#define PORT 5000 
#define SA struct sockaddr

int read_thread_on(1);
int write_thread_on(1);
int timer_thread_on(1);

void * heartBeat(void * arg)
{
	int sockfd = *(int *) arg;
//	cout << "start heartBeat()" << endl;
//	cout.flush();
	timer_thread_on = 1;
	while (timer_thread_on ) {
		int ret = send(sockfd, "heartBeat\n", 10, 0);
		//usleep(15 * 1000000);
		for (int idx = 0; idx < 15; idx++ ) {
			if (timer_thread_on == 0) break;
			usleep (1000000);
		}
	}
	cout << "Leaving heartBeat()." << endl;
	cout.flush();
	return (void*)nullptr;	
}

void * read_func (void * arg)
{
	int sockfd = *(int *) arg;
//	printf ("start, use sock: %d\n", sockfd);
	char buffer[MAX];

//	cout << "start read_func()" << endl;
//	cout.flush();

	read_thread_on = 1;
	while (read_thread_on) {
		bzero(buffer, sizeof(buffer));
		int retval = read(sockfd, buffer, sizeof(buffer));
//		cout << "retval: " << retval << endl;
		if (retval) {

			int idx(0);
            while( buffer[idx] ) {
				char c = buffer[idx];
				tolower(c);
				idx++;
			}

			if ((strncmp(buffer, "close", 5)) == 0) {
				write_thread_on = 0;
				read_thread_on = 0;
				timer_thread_on = 0;
				//printf("Client Close...\n");
				break;
			} else
			if ((strncmp(buffer, "exit", 4)) == 0) {
				write_thread_on = 0;
				read_thread_on = 0;
				timer_thread_on = 0;
				//printf("Client Exit...\n");
				break;
			}
		}
		sleep(.5);
	}
	cout << "Leaving read_func()." << endl;
	cout.flush();
	//pthread_exit(NULL);
	return (void*) nullptr;
}

void * write_func(void * arg)
{
	int sockfd = *(int *) arg;
	char buffer[MAX];
	
//	cout << "start write_func()" << endl;
//	cout.flush();

	write_thread_on = 1;
	while (write_thread_on) {
		int n = 0;
		printf("Enter the string : ");
		bzero(buffer, sizeof(buffer));
		while ((buffer[n++] = getchar()) != '\n')
			;
		if (buffer[0] == '\n') continue;

		//write(sockfd, buffer, strlen(buffer));
		send(sockfd, buffer, strlen(buffer), 0);
		//cout << " to sockfd: " << sockfd << " length: " << strlen(buffer) << " write: " << buffer;

		// ----------
		// rtr - added 
		// "Close" the current active connection
		// "Exit" the app
		//

		int idx(0);

		while( buffer[idx] ) {
			char c = buffer[idx];
			tolower(c);
			idx++;
		}

		if ((strncmp(buffer, "close", 5)) == 0) {
			write_thread_on = 0;
			read_thread_on = 0;
			timer_thread_on = 0;
			break;
		} else
		if ((strncmp(buffer, "exit", 4)) == 0) {
			write_thread_on = 0;
			read_thread_on = 0;
			timer_thread_on = 0;
 			break;    // exit the current thread
		}
	}
	cout << "Leaving write_func()." << endl;
	cout.flush();

	//pthread_exit(NULL);
	return (void*) nullptr;
}

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	} else {
		printf("Socket successfully created..\n");
	}
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	} else {
		printf("connected to the server... sockfd: %d\n", sockfd);
	}
	
	pthread_t threads[3];
	int i = 0;
	
	// ----------
	// rtr - have keyboard input in background to allow for server talking to client.
	//
	if (pthread_create(&threads[0], NULL, &write_func, &sockfd)) {
        	cout << "Error in thread creation of write(), sockfd: " << sockfd << endl;
		close(sockfd);
		exit(1);
	}

	// ----------

	// ----------
	// rtr - read from server...
	//
	if (pthread_create(&threads[1], NULL, &read_func, &sockfd)) {
        	cout << "Error in thread creation of read_func()" << endl;
		close(sockfd);
		exit(1);
	}

	if (pthread_create(&threads[2], NULL, &heartBeat, &sockfd)) {
		cout << "Error in thread creation of heartBeat, sockfd: " << sockfd << endl;
		close(sockfd);
		exit(1);
	}

	while (!timer_thread_on) {
		sleep(.5);
	}
//	cout << "----- start timer -----" << endl;
//	cout << "heartBeat() thread created" << endl;

	while (!read_thread_on) {
		sleep(.5);
	}
//	cout << "read_func() thread created" << endl;	

	while (!write_thread_on) {
		sleep(.5);
	}
//	cout << "write_func() thread created" << endl;	
	
	while (read_thread_on || write_thread_on || timer_thread_on) {
		sleep(1);
	}
	//sleep(1);

	// close the socket
	close(sockfd);
}

