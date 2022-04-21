
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close

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

/*
//#include <stdio.h>
//#include <string.h> //strlen
//#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
*/

int read_thread_on(1);
int write_thread_on(1);

#if 0
void * heartBeat(void * arg)
{
	int sockfd = *(int *) arg;

	//for (;;) {
	while (write_thread_on || read_thread_on) {
		//pthread_mutex_lock(&mutex);
		int ret = write(sockfd, "heartBeat", 9);
		//pthread_mutex_lock(&mutex);

		cout << "\n==> in thread loop, write ret: " << ret << endl;

		cout.flush();
		usleep(15 * 1000000);
	}
	return (void*)nullptr;	
}
#endif

void * read (void * arg)
{
	int sockfd = *(int *) arg;
	char buff[MAX];
#if 1
	read_thread_on = 1;
	while (read_thread_on) {
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			read_thread_on = 0;
		}
	}
#endif
	return (void*) nullptr;
}
//void func(int sockfd)
void * write(void * arg)
{
	int sockfd = *(int *) arg;
	char buff[MAX];
	int n;
	
	write_thread_on = 1;
	while (write_thread_on) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));

		// ----------
		// rtr - added 
		// "Close" the current active connection
		// "Exit" the app
		//
		string msg(buff);
		cout << "wrote to server: " << msg.c_str();
		transform(msg.begin(), msg.end(), msg.begin(), ::tolower);
		if (msg.compare(0, msg.length() - 1, "close") == 0) {
			cout << "msg: " << msg.c_str();
			write_thread_on = 0;
			break;
		} else
		if (msg.compare(0, msg.length() - 1, "exit") == 0) {
			cout << "msg: " << msg.c_str() << endl;			
			write_thread_on = 0;
 			break;    // exit the current thread
		}
		//} else {
		//
		// ----------
		//	bzero(buff, sizeof(buff));
		//	read(sockfd, buff, sizeof(buff));
		//	printf("From Server : %s", buff);
		//	if ((strncmp(buff, "exit", 4)) == 0) {
		//		printf("Client Exit...\n");
		//		break;
		//	}
		//}
	}

	cout << "thread thread_on: " << write_thread_on << endl;
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
		printf("connected to the server..\n");
	}
	
	pthread_t threads[3];
#if 0
	// -----
	// rtr - start timer thread
	cout << "----- start timer -----" << endl;
	cout.flush();
	int i(0);
	pthread_t threads[3];
    if (pthread_create(&threads[0], NULL, &heartBeat, &sockfd)) {
        cout << "Error in thread creation of heartBeat " << endl;
	    close(sockfd);
	    exit(1);
    }
	while (write_thread_on && read_thread_on) {
		sleep(.5);
	}
	cout << "heartBeat thread created" << endl;
	cout.flush();
//sleep(5000000);

	// -----
#endif
	
	// function for chat
	//write(sockfd);
#if 1
	// ----------
	// rtr - have keyboard input in background to allow for server talking to client.
	//
    if (pthread_create(&threads[1], NULL, &write, &sockfd)) {
        cout << "Error in thread creation of write()" << endl;
	    close(sockfd);
	    exit(1);
    }
	//sleep(1);
	while (!write_thread_on) {
		sleep(1);
	}
	
	cout << "write() thread created" << endl;	
	cout << "wait for exiting write_thread_on: " << write_thread_on << endl;
	cout.flush();

//	while (!write_thread_on) {
//		sleep(1);
//	}

	while (write_thread_on) {
		sleep(1);
	}
	// ----------
#endif

#if 0
	// ----------
	// rtr - have keyboard input in background
	//
:wait    if (pthread_create(&threads[1], NULL, &read, &sockfd)) {
        cout << "Error in thread creation of read()" << endl;
	    close(sockfd);
	    exit(1);
    }
	cout << "read() thread created" << endl;	
	cout << "wait for exiting read_thread_on: " << read_thread_on << endl;
	cout.flush();

	while (!read_thread_on) {
		sleep(1);
	}
	
	while (read_thread_on) {
		sleep(1);
	}
	// ----------
#endif

	// close the socket
	close(sockfd);

	//	write_thread_on = 0;
	//while (write_thread_on) {
	//	sleep(1);
	//}
}

