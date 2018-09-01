#ifndef SOCKET_H
#define SOCKET_H

#define PORT 46564
#define MESSAGE_BUFFER_SIZE 1024

// write array to socket (send)
int sendArray(int socket_fd, char* message, long length);

// write string to socket (send)
// note that the length is determined by the position of the first null character (strlen)
int sendString(int socket_fd, char* message);

// wait for response and read data into a char array, length read is returned through len
char* receiveArray(int socket_fd, long* len);

// wait for response and read it into buffer but no length is returned
char* receiveString(int socket_fd);

// connect the client to a server
int connectToServer(char* server_ip);

#endif