//SCREENSHOT
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <string.h>
#include <stdio.h>

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
extern int h_errno;

#define BUF_SIZE 255

int connect_to_server(char *hostname){
	int socket_desc;
	struct sockaddr_in server;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	struct hostent *he = gethostbyname(hostname);	
	memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);	
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect error");
		return 1;
	}

	printf("Connected\n");
	return socket_desc;
}

int get_screenshot(int fd, char *host)
{
	int socket_desc = connect_to_server(host);
	
	char buffer[BUF_SIZE];
	int n = 0;
	while((n = read(socket_desc, buffer, BUF_SIZE)) > 0){
		if(write(fd, buffer , n) < 0){
			printf("receive _failed");
			return 0;
		}
	}
	printf("Data Received\n");
	close(socket_desc);
	return 1;
}


int main(int argc, char **argv) {
	char *host;
	if(argc > 1)
		host = argv[1];
	else
		host = "bouleau02";
	get_screenshot(open("screenshot.png", O_CREAT | O_WRONLY | O_RDONLY), host);
	return 0;
}
