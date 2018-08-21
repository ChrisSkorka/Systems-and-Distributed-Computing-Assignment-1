#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define PORT 42069
#define BUFFER_SIZE 256

void main(int argc, char** argv){

    if(argc < 2){
        printf("IP argument required\n");
        return;
    }

    // variables
    int fd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_SIZE];

    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error opening socket\n");
        return;
    }

    // set host
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("Error no such host\n");
        return;
    }

    // connect
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
    serv_addr.sin_port = htons(PORT);

    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Error connecting");
    }

    while(1){
        printf("> ");
        bzero(buffer, BUFFER_SIZE);
        fgets(buffer, 255, stdin);
        n = write(fd, buffer, strlen(buffer));
        if (n < 0){
            printf("Error writing to socket\n");
            continue;
        }
            
        bzero(buffer, BUFFER_SIZE);
        n = read(fd, buffer, 255);
        if (n < 0){
            printf("Error reading from socket\n");
            continue;
        }
            
        printf("%s\n",buffer);
    }




}