#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
	int network_socket;	// socket fd
	char *address;

	address = argv[1];

	network_socket = socket(AF_INET, SOCK_STREAM, 0);	//create a socket
	if(network_socket == -1){
		perror("socket error");
	}
	
	struct sockaddr_in s_address;
	s_address.sin_family = AF_INET;				// IPV4
	s_address.sin_port = htons(8099);				// spec asks set port:80
	s_address.sin_addr.s_addr = inet_addr("127.0.0.1");			// connect to local machine (able to connect real IP)
	inet_aton(address, (struct in_addr *) &s_address.sin_addr.s_addr);

	int connect_status = connect(network_socket, (struct sockaddr *) &s_address, sizeof(s_address));

	if(connect_status == -1){
		perror("connection failure");
	}

	char request[] = "GET / HTTP/1.1\r\n\r\n";
	char response[4096];

	// send message to socket
	send(network_socket, request, sizeof(request), 0);

	// recieve message from socket
	recv(network_socket, response, sizeof(response), 0);
	
	printf("response from the server : \n%s\n", response);
	close(network_socket);

	return 0;
}
