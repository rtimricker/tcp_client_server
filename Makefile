

all: tcp_client.cpp tcp_server.cpp tcp_server tcp_client Makefile

tcp_server: tcp_server.cpp Makefile
	g++ -o tcp_server tcp_server.cpp -lpthread

tcp_client: tcp_client.cpp Makefile
	g++ -o tcp_client tcp_client.cpp -lpthread


clean:
	rm tcp_server
	rm tcp_client
