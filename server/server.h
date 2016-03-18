#ifndef __SERVER_H__
#define __SERVER_H__

//SCREENSHOT
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

//FILE
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

//NETWORK
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

//UTIL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "signals.h"
#include "socket.h"

#define BUF_SIZE 255
#define SERVER_PORT 8080
#define NAME "server"
#define SCREEN_FILE "/tmp/screen.png"
#define SHELL_CMD "bash"


#endif
