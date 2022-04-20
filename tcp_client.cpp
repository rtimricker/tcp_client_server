
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

#if 0
void * heartBeat(void * arg)
{
	int sockfd = *(int *) arg;

	for (;;) {
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

void func(int sockfd)
{
	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
		// ----------
		// rtr - added 
		// "Close" the current active connection
		//
		string msg(buff);
		transform(msg.begin(), msg.end(), msg.begin(), ::tolower);
		if (msg.compare(0, msg.length() - 1, "close") == 0) {
			break;
		}
		//
		// ----------
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
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
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

#if 0
	// -----
	// rtr - start timer thread
	cout << "----- start timer -----" << endl;
	cout.flush();
	int i(0);
	pthread_t threads[i];
        if (pthread_create(&threads[0], NULL, &heartBeat, &sockfd)) {
            cout << "Error in thread creation: " << endl;
	    close(sockfd);
	    exit(1);
        }
	cout << "thread created" << endl;
	cout.flush();
sleep(5000000);

	// -----
#endif
	
	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}

