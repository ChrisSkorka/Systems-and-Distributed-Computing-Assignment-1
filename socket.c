#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netdb.h>
    #include <netinet/in.h>
#endif

#define PORT 46564
#define MESSAGE_BUFFER_SIZE 1024

// write array to socket (send)
int sendArray(int socket_fd, char* message, long length){

    // get string representation of length of message
    char message_size_str[MESSAGE_BUFFER_SIZE];
    sprintf(message_size_str, "%ld", length);

    // send size of actual message
    int code = send(socket_fd, message_size_str, MESSAGE_BUFFER_SIZE, 0);
    if(code < 0){
        printf("Error writing to socket\n");
        return code;
    }

    // send actual message
    code = send(socket_fd, message, length, 0);
    if(code < 0){
        printf("Error writing to socket\n");
        return code;
    }
    
    return code;
}

// write string to socket (send)
// note that the length is determined by the position of the first null character (strlen)
int sendString(int socket_fd, char* message){
    // message length
    long length = strlen(message) + 1;
	// send message
	return sendArray(socket_fd, message, length);
}

// wait for response and read data into a char array, length read is returned through len
char* receiveArray(int socket_fd, long* len){

    // read response size
    char message_size_str[MESSAGE_BUFFER_SIZE];
    // bzero(message_size_str, MESSAGE_BUFFER_SIZE);
    int code = recv(socket_fd, message_size_str, MESSAGE_BUFFER_SIZE, 0);
    if (code < 0)
        printf("Error reading from socket\n");

    // create string to contain message
    int length = atoi(message_size_str);
    *len = length;
    char* message = calloc(length + 1, sizeof(char));

    // read actual response
    code = recv(socket_fd, message, length, 0);
    if (code < 0)
        printf("Error reading from socket\n");

    return message;
}

// wait for response and read it into buffer but no length is returned
char* receiveString(int socket_fd){
	long len;
	return receiveArray(socket_fd, &len);
}

// connect to a server
int connectToServer(char* server_ip){
    
    // variables
    int fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error opening socket\n");
        return 0;
    }

    // get host from args into server struct
    server = gethostbyname(server_ip);
    if (server == NULL) {
        printf("Error no such host\n");
        return 0;
    }

    // setup serv_addr struct for socket
    // bzero((char *) &serv_addr, sizeof(serv_addr));
    // serv_addr.sin_family = AF_INET;
    // bcopy((char *)server->h_addr, 
    //     (char *)&serv_addr.sin_addr.s_addr,
    //     server->h_length);
    serv_addr.sin_port = htons(PORT);

    // connect to socket
    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Error connecting\n");
        return 0;
    }

    return fd;
}
