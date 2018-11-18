#ifndef _SOCKET_H__
#define _SOCKET_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT_NUM 8000
#define BUFFER_SIZE 4096

int getSocketFD(void);

void init_Socket(struct sockaddr_in *, char *address);

#endif

