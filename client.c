#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket.h"

#define PORT_NUM 8000
#define BUFFER_SIZE 4096

void init_Client(int, char **, char *);
void connect_Socket(int , struct sockaddr_in *);
void socket_Handler(int, char *, int);

int main(int argc, char *argv[]){
	int clientSocket;	// socket fd
	char socketAddress[128];
	char response[] = "GET / HTTP/1.1\r\n\r\n";
	int messageLength = strlen(response);
	struct sockaddr_in clientInfo;

	init_Client(argc, argv, socketAddress);

	clientSocket = getSocketFD();

	init_Socket(&clientInfo, socketAddress);

	connect_Socket(clientSocket, &clientInfo);

	socket_Handler(clientSocket, response, messageLength);

	close(clientSocket);

	return 0;
}

void init_Client(int argc, char **argv, char *socketAddress){
	if(argc != 2){
		fprintf(stderr, "Not enough argument\n");
		exit(EXIT_FAILURE);
	}
	strcpy(socketAddress,argv[1]);
}

void connect_Socket(int socketFD, struct sockaddr_in *socketInfo){
	
	if((connect(socketFD, (struct sockaddr *)socketInfo, sizeof(struct sockaddr_in))) == -1){
		perror("connect");
		close(socketFD);
		exit(EXIT_FAILURE);
	}
}

void socket_Handler(int socketFD, char *response, int messageLength){
	
	char recieve[BUFFER_SIZE];

	send(socketFD, response, messageLength, 0);
	recv(socketFD, recieve, BUFFER_SIZE, 0);
	printf("Recieve:\n%s\n", recieve);
	return;
}
