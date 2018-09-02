// /////////////////////////////////////////////////////////////////////////////
// Filename:        socket.c
// Author:          Christopher Skorka
// Date Created:    30/08/2018
// Description:     Socket IO, sends and receive messages and connect client 
//                  to the server
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include "socket.h"
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

// PROTOTYPES //////////////////////////////////////////////////////////////////
int sendArray(int socket_fd, char* message, long length);
int sendString(int socket_fd, char* message);
char* receiveArray(int socket_fd, long* len);
char* receiveString(int socket_fd);
int connectToServer(char* server_ip);

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// send array of given length through provided socket
// Parameters:  int socket_fd:  socket file descriptor throug which to send
//              char* message:  message to be sent
//              long length:    number of bytes to send
// Returns:     int:            number of bytes sent or -1 on failure
// -----------------------------------------------------------------------------
int sendArray(int socket_fd, char* message, long length){

    // get string representation of length of message
    char message_size_str[MESSAGE_BUFFER_SIZE];
    sprintf(message_size_str, "%ld", length);

    // send size of actual message
    int code = 0;
    long message_size_length_sent = 0;
    while(code >= 0 && message_size_length_sent < MESSAGE_BUFFER_SIZE){

        // send parts until entire message is sent or failure
        int start = (int) message_size_length_sent;
        int remaining = MESSAGE_BUFFER_SIZE - message_size_length_sent;
        code = send(socket_fd, &message_size_str[start], remaining, 0);
        if(code < 0){
            printf("Error writing to socket\n");
            return code;
        }else
            message_size_length_sent += code;
    }

    // send actual message
    code = 0;
    long message_length_sent = 0;
    while(code >= 0 && message_length_sent < length){

        // send parts until entire message is sent or failure
        int start = (int) message_length_sent;
        int remaining = length - message_length_sent;
        code = send(socket_fd, &message[start], remaining, 0);
        if(code < 0){
            printf("Error writing to socket\n");
            return code;
        }else
            message_length_sent += code;
    }
    
    // code is bytes read or -1 on failure
    return code;
}

// -----------------------------------------------------------------------------
// send string through provided socket, the length of the message is determined 
// by the position of the first null character (strlen)
// Parameters:  int socket_fd:  socket file descriptor throug which to send
//              char* message:  message to be sent
// Returns:     int:            number of bytes sent or -1 on failure
// -----------------------------------------------------------------------------
int sendString(int socket_fd, char* message){
    // message length
    long length = strlen(message) + 1;
	// send message
	return sendArray(socket_fd, message, length);
}

// -----------------------------------------------------------------------------
// wait for response and read data into a char array, length red is returned 
// through the len pointer
// Parameters:  int socket_fd:  socket file descriptor from which to read
//              long* len:  store number of bytes recieved (side effect)
// Returns:     char*:      char array recieved
// -----------------------------------------------------------------------------
char* receiveArray(int socket_fd, long* len){

    // read response size
    char message_size_str[MESSAGE_BUFFER_SIZE];
    int code = 0;

    // read until default message size has been red
    int size_len_recieved = 0;
    while(size_len_recieved < MESSAGE_BUFFER_SIZE && code > -1){

        // read parts until entire message is sent or failure
        code = recv(socket_fd, message_size_str, MESSAGE_BUFFER_SIZE, 0);
        if (code < 0){ // 
            printf("Error reading from socket\n");
            return NULL;
        }else
            size_len_recieved += code;
    }

    // create string to contain message
    int length = atoi(message_size_str);
    *len = length;
    char* message = calloc(length + 1, sizeof(char));

    // read actual response
    int len_recieved = 0;
    while(len_recieved < length && code > -1){

        // read parts until entire message is sent or failure
        code = recv(socket_fd, &message[len_recieved], length, 0);
        if (code < 0){
            printf("Error reading from socket\n");
            return NULL;
        }else
            len_recieved += code;
    }

    return message;
}

// -----------------------------------------------------------------------------
// wait for response and read it into buffer but no length is returned
// Parameters:  int socket_fd:  socket file descriptor from which to read
// Returns:     char*:          char array recieved
// -----------------------------------------------------------------------------
char* receiveString(int socket_fd){
	long len; // dummy length variable
	return receiveArray(socket_fd, &len);
}

// -----------------------------------------------------------------------------
// connect the client to a server
// Parameters:  char* server_ip:    server address to connect to
// Returns:     int:                socket file descriptor of connection
// -----------------------------------------------------------------------------
int connectToServer(char* server_ip){
    
    // variables
    int fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // setup sockets for windows
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsa;
        if(WSAStartup(MAKEWORD(2,2),&wsa) != 0){
            printf("Failed setting up socket for win. Error Code : %d", WSAGetLastError());
            return 0;
        }
    #endif

    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        // WSAGetLastError();
        printf("Error opening socket, error code: %d\n", fd);
        return 0;
    }

    // get host from args into server struct
    server = gethostbyname(server_ip);
    if (server == NULL) {
        printf("Error no such host\n");
        return 0;
    }

    // setup serv_addr struct for socket
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr,
        (char *)server->h_addr,
        server->h_length);
    serv_addr.sin_port = htons(PORT);

    // connect to socket
    if (connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Error connecting\n");
        return 0;
    }

    // socket file descriptor
    return fd;
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////