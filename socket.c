#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MESSAGE_BUFFER_SIZE 1024

// write array to socket (send)
int sendArray(int socket_fd, char* message, long length){

    // get string representation of length of message
    char message_size_str[MESSAGE_BUFFER_SIZE];
    sprintf(message_size_str, "%ld", length);

    // send size of actual message
    int code = write(socket_fd, message_size_str, MESSAGE_BUFFER_SIZE);
    if(code < 0){
        printf("Error writing to socket\n");
        return code;
    }

    // send actual message
    code = write(socket_fd, message, length);
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
    bzero(message_size_str, MESSAGE_BUFFER_SIZE);
    int code = read(socket_fd, message_size_str, MESSAGE_BUFFER_SIZE);
    if (code < 0)
        printf("Error reading from socket\n");

    // create string to contain message
    int length = atoi(message_size_str);
    *len = length;
    char* message = calloc(length + 1, sizeof(char));

    // read actual response
    code = read(socket_fd, message, length);
    if (code < 0)
        printf("Error reading from socket\n");

    return message;
}

// wait for response and read it into buffer but no length is returned
char* receiveString(int socket_fd){
	long len;
	return receiveArray(socket_fd, &len);
}
