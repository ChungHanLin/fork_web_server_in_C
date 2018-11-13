#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define Port_Num 8099

void write_protocol(char *http_header);
char *get_time(char *);

static void wait_child(int singNum){
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void socket_handler(int socket_fd, char *recieve, char *response, int length){
	send(socket_fd, response, length, 0);
	recv(socket_fd, recieve, 1024, 0);
	close(socket_fd);
}

int main(int argc, char *argv[]){
	
	//open html file
	int htmlFD = open("index.html", O_RDONLY);
	char htmlContent[4096];
	char recieve[1024];
	int length = read(htmlFD, htmlContent, 4096);
	
	htmlContent[length] = '\0';

	char response[2048];
	write_protocol(response);

	length += strlen(response);
	strcat(response, htmlContent);

	int network_socket, new_socket, pid;
	struct sigaction s;

	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(network_socket == -1){
		perror("socket error");
	}

	struct sockaddr_in serverInfo, clientInfo;
	int client_addrlen = sizeof(clientInfo);
	int status;	// waiting for child process to complete

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = INADDR_ANY;
	serverInfo.sin_port = htons(Port_Num);

	if(bind(network_socket, (struct sockaddr *) &serverInfo, sizeof(serverInfo)) == -1){
		perror("bind error");
	}

	if(listen(network_socket, 5) == -1){
		perror("listen error");
	}

	s.sa_handler = wait_child;
	sigemptyset(&s.sa_mask);
	s.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &s, NULL) == -1){
		perror("sigaction error");
	}


	while(1){
		// get accepted socket
		new_socket = accept(network_socket, (struct sockaddr *) &clientInfo, (unsigned int *) &client_addrlen);
		if(new_socket == -1){
			perror("client_socket error");
			exit(EXIT_FAILURE);
		}
		socket_handler(new_socket, recieve, response, length);
		fprintf(stderr, "execute\n");
		// do fork
		pid = fork();
		if(pid < 0){
			perror("fork error");
		}
		else if(pid == 0){	// child process
			//close(network_socket);
			socket_handler(new_socket, recieve, response, length);
		}
		else{			// parent process
			//close(new_socket);
			break;
		}
		
	}

	close(network_socket);
	printf("done\n");
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

void write_protocol(char *http_header){
	char header[] = "HTTP/1.1 200 OK\r\n";
	char host[] = "Host:localhost\r\n";
	char port[] = "Port:8001\r\n";
	char time[512];
	char *time_Reg = get_time(time);

	strcat(http_header, header);
	strcat(http_header, host);
	//strcat(http_header, Port_Num);
	strcat(http_header, time_Reg);
	strcat(http_header, "\r\n\n");
}
