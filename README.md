# TINY-RAT

### PURPOSE
This little remote access tool has been developed by Nicolas Pamart and Dylan Guerville : It is a project for cyber-security introduction course we followed.

You can remotely access to target's shell, take screenshots of his screen and update the rat.

Server is destinated to be run on the target computer while client is our tool to control it.

### BUILD
You will need to have cairo installed on your computer in order to compile it.
Visit http://cairographics.org/download/ to download and help on how to install it.

Type `make` in project's root folder to compile client and server.
They can be compiled independently by taping `make` being located in client or server folder.

### RUN
- In order to run the server type `./server` in server folder.
- In order to run the client type `./client [server_hostname]` in client folder. If no hostname is specified, default is localhost.

### WHAT TO KNOW

You can modify a lot of things in the server.h header before compiling :


- `SERVER_PORT` : the port the server will listen to, default is 8080
- `NAME` : the server's executable name, change it to your executable name in order to let it auto-update, default is "server".
- `SCREEN_FILE` : the screenshot's location (temporary stored as we sent it directly to client) default is "/tmp/screen.png"
- `SHELL_CMD` : Shell command to run, default is "bash"


For the client, you can modidy some vars in client.c :


- `PORT` : the server port the client will connect to, default is 8080
- `SCREEN_FILE` : the location to store screenshot, default is "screenshot.png"
- `UPDATE_FILE` : the file to send to server in order to update it, default is "update"
