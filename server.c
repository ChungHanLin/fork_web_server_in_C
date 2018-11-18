#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "socket.h"

#define PORT_NUM 8000
#define BUFFER_SIZE 4096
#define BACKLOG_NUM 5


void write_Protocol(char *);
char *get_time(char *);
int get_Request(char *response);

void bind_Server(int, struct sockaddr_in *);
void listen_Server(int);
void signal_Action(int ,struct sigaction *);
int accept_Client(int, struct sockaddr_in *);
void socket_Handler(int, char *, int);

static void wait_child(int singNum){
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]){
	int serverSocket, clientSocket;
	int replyLength = 0;
	char recieve[BUFFER_SIZE], response[BUFFER_SIZE];
	struct sockaddr_in serverInfo, clientInfo;
	struct sigaction s;

	serverSocket = getSocketFD();

	replyLength = get_Request(response);
	
	init_Socket(&serverInfo, NULL);

	bind_Server(serverSocket, &serverInfo);

	listen_Server(serverSocket);

	signal_Action(serverSocket, &s);

	while(1){
		int pid;

		clientSocket = accept_Client(serverSocket, &clientInfo);

		fprintf(stderr, "execute\n");

		pid = fork();

		if(pid < 0){
			perror("fork error");
			close(clientSocket);
			close(serverSocket);
			exit(EXIT_FAILURE);
		}

		else if(pid == 0){
			socket_Handler(clientSocket, response, replyLength);
		}

		close(clientSocket);
	}

	close(serverSocket);
	return 0;
}

char *get_time(char *c_time_string){

	time_t current_time;

	current_time = time(NULL);

	if (current_time == ((time_t)-1)){
        	fprintf(stderr, "Failure to obtain the current time.\n");
		exit(EXIT_FAILURE);    	
	}

   	/* Convert to local time format. */
    	c_time_string = ctime(&current_time);

    	if (c_time_string == NULL){
        	fprintf(stderr, "Failure to convert the current time.\n");
        	exit(EXIT_FAILURE);
    	}
	return c_time_string;
}

void write_Protocol(char *http_header){
	char header[] = "HTTP/1.1 200 OK\r\n";
	char host[] = "Host:localhost\r\n";
	char port[] = "Port:";
	char time[512];
	char *time_Reg = get_time(time);

	sprintf(http_header, "%s%s%s%d\r\n%s\r\n\n", header, host, port, PORT_NUM, time_Reg);
}

int get_Request(char *response){
	int htmlFD = open("index.html", O_RDONLY);
	char htmlContent[BUFFER_SIZE];
	int length = read(htmlFD, htmlContent, BUFFER_SIZE);

	htmlContent[length] = '\0';
	write_Protocol(response);

	length += strlen(response);
	strcat(response, htmlContent);

	return length;
}

void bind_Server(int socketFD, struct sockaddr_in *serverInfo){

	if(bind(socketFD, (struct sockaddr *) serverInfo, sizeof(struct sockaddr_in)) == -1){
		perror("bind");
		close(socketFD);
		exit(EXIT_FAILURE);
	}
}

void listen_Server(int socketFD){

	if(listen(socketFD, BACKLOG_NUM) == -1){
		perror("listen");
		close(socketFD);
		exit(EXIT_FAILURE);
	}
}

void signal_Action(int socketFD, struct sigaction *s){
	
	s->sa_handler = wait_child;
	sigemptyset(& (s->sa_mask));
	s->sa_flags = SA_RESTART;

	if(sigaction(SIGCHLD, s, NULL) == -1){
		perror("sigaction");
		close(socketFD);
		exit(EXIT_FAILURE);
	}
}

int accept_Client(int serverSocketFD, struct sockaddr_in *clientInfo){
	
	int clientSocketFD;
	socklen_t length = sizeof(struct sockaddr_in);

	clientSocketFD = accept(serverSocketFD, (struct sockaddr *) clientInfo, &length);

	if(clientSocketFD == -1){
		perror("accept");
		close(serverSocketFD);
		exit(EXIT_FAILURE);
	}
}

void socket_Handler(int socketFD, char *response, int messageLength){
	
	char recieve[BUFFER_SIZE];

	recv(socketFD, recieve, BUFFER_SIZE, 0);
	send(socketFD, response, messageLength, 0);
	printf("Recieve:\n%s\n", recieve);
	return;
}
