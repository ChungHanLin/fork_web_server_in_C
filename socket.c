#include "socket.h"

int getSocketFD(void){

	int socketFD;

	socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if(socketFD == -1){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	return socketFD;
}

void init_Socket(struct sockaddr_in *socketInfo, char *socketAddress){
	socketInfo->sin_family = AF_INET;
	socketInfo->sin_port = htons(PORT_NUM);
	if(socketAddress){
		socketInfo->sin_addr.s_addr = inet_addr(socketAddress);
		inet_aton(socketAddress, (struct in_addr *) &(socketInfo->sin_addr.s_addr));
	}
	else{
		socketInfo->sin_addr.s_addr = INADDR_ANY;
	}
}


