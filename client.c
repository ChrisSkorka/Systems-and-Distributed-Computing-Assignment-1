#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define PORT 42069
#define BUFFER_SIZE 256

void main(int argc, char** argv){

    // require server name
    if(argc < 2){
        printf("IP argument required\n");
        return;
    }

    // variables
    int fd, code;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];

    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error opening socket\n");
        return;
    }

    // get host from args into server struct
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("Error no such host\n");
        return;
    }

    // setup serv_addr struct for socket
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
    serv_addr.sin_port = htons(PORT);

    // connect to socket
    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Error connecting");
        return;
    }

    // continuous cpmmand execution loop
    while(1){
        // query for command
        printf("> ");
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, 255, stdin);

        // if command is exit, break loop before sending any command
        if(strcmp(buffer, "exit\n") == 0)
            break;

        // write entered string to socket (send)
        code = write(fd, buffer, strlen(buffer));
        if (code < 0){
            printf("Error writing to socket\n");
            continue;
        }
            
        // wait for response and read it into buffer
        bzero(buffer, BUFFER_SIZE);
        code = read(fd, buffer, 255);
        if (code < 0){
            printf("Error reading from socket\n");
            continue;
        }
        
        // print server response
        printf("%s\n",buffer);
    }
}