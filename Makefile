

#all: tcp_server.cpp tcp_client.coo

server: tcp_server.cpp
	g++ -o tcp_server tcp_server.cpp -lpthread
client: tcp_client.cpp
	g++ -o tcp_client tcp_client.cpp -lpthread
