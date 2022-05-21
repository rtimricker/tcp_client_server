//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
//#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <time.h>
	
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

using namespace std;
		
#define TRUE 1
#define FALSE 0
//#define PORT 8888
#define PORT 5000
	
int main(int argc , char *argv[])
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[30] , client_timeout[30],
		max_clients = 30 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
		
	char buffer[1025]; //data buffer of 1K
		
	//set of socket descriptors
	fd_set readfds;
		
	//a message
	//const char * message = "ECHO Daemon v1.0 \r\n";
	const char * message = "Test Output \r\n";
	
	// initialise all client_socket[] to 0 so not checked
	memset(client_socket, 0, sizeof(client_socket));
	memset(client_timeout, 0, sizeof(client_timeout));
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
		
	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
		
	while(TRUE) {
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++) {
			//socket descriptor
			sd = client_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
	
		// wait for an activity on one of the sockets

		timeval timeout;
		timeout.tv_sec = 1; 
    	timeout.tv_usec = 0; 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , (timeval*)&timeout);
	
		if (activity == 0) {
			// a timeout occurer on socket
//			printf("timeout occured on server\n");
			// check time delay of any particular socket,
			// if any at 30 or more seconds, disconnect that socket

			for (int idx = 0; idx < max_clients; ++idx) {
				if (client_timeout[idx] >= 30 && client_socket[idx]) {
					printf ("server: socket %d at index %d timed out at %d seconds, closing\n", client_socket[idx], idx, client_timeout[idx]);
					
					send(client_socket[idx], "exit\n", 5, 0);
					close (client_socket[idx]);
					usleep(1000);
					client_timeout[idx] = 0;
					client_socket[idx] = 0;
				} else {
					++client_timeout[idx];
				}
			}
		}

		if ((activity < 0) && (errno!=EINTR)) {
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds)) {
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , 
			new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
		
			//send new connection greeting message
			if( send(new_socket, message, strlen(message), 0) != strlen(message) ) {
				perror("send");
			}
				
			puts("Welcome message sent successfully");
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++) {
				//if position is empty
				if( client_socket[i] == 0 ) {
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++) {
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds)) {
				//Check if it was for closing , and also read the
				//incoming message
				bzero(buffer, sizeof(buffer));
				if ((valread = read( sd , buffer, 1024)) == 0) {
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
					inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}
					
				//Echo back the message that came in
				else {
					cout << "buffer sd: " << sd << " from client: " << buffer;

					// ----------
					// rtr - added: On receipt of a "Close" close/disconnect this active socket.
					// turn buffer to lower case...
					
					int idx(0);
					while( buffer[idx] ) {
                                		char c = buffer[idx];
                                		//putchar(tolower(c));
                                		tolower(c);
                                		idx++;
					}
					
					if ((strncmp(buffer, "close", 4)) == 0) {
						printf("Close. Host disconnected , ip %s , port %d \n" ,
							inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						//Close the socket and mark as 0 in list for reuse
						send(sd, buffer, strlen(buffer), 0 );
						close( sd );
						client_socket[i] = 0;
						break;
					} else if ((strncmp(buffer, "exit", 4)) == 0) {
						printf("Exit. Host disconnected , ip %s , port %d \n" ,
							inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
						cout << "exiting all ..." << endl;
						for (int idx = 0; idx < max_clients; idx++) {
							int _sd = client_socket[idx];
							//cout << "idx: " << idx << " _sd: " << _sd << endl;
							if (_sd) {
								// send to other clients, let each close their socket
								cout << "send to _sd: " << _sd << " buffer: " << buffer;
								send(_sd , buffer, strlen(buffer), 0 );
								close( _sd );	
								client_socket[idx] = 0;
							}
						}
						return 1;
					}
				}
			}
		}
	}
		
	return 0;
}

