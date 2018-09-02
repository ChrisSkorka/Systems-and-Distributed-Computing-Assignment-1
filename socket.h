// /////////////////////////////////////////////////////////////////////////////
// Filename:        socket.c
// Author:          Christopher Skorka
// Date Created:    30/08/2018
// Description:     Socket IO, sends and receive messages and connect client 
//                  to the server
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef SOCKET_H
#define SOCKET_H

// GLOBALS /////////////////////////////////////////////////////////////////////
#define PORT 46564
#define MESSAGE_BUFFER_SIZE 1024

// PROTOTYPES //////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// send array of given length through provided socket
// Parameters:  int socket_fd:  socket file descriptor throug which to send
//              char* message:  message to be sent
//              long length:    number of bytes to send
// Returns:     int:            number of bytes sent or -1 on failure
// -----------------------------------------------------------------------------
int sendArray(int socket_fd, char* message, long length);

// -----------------------------------------------------------------------------
// send string through provided socket, the length of the message is determined 
// by the position of the first null character (strlen)
// Parameters:  int socket_fd:  socket file descriptor throug which to send
//              char* message:  message to be sent
// Returns:     int:            number of bytes sent or -1 on failure
// -----------------------------------------------------------------------------
int sendString(int socket_fd, char* message);

// -----------------------------------------------------------------------------
// wait for response and read data into a char array, length red is returned 
// through the len pointer
// Parameters:  int socket_fd:  socket file descriptor from which to read
//              long* len:  store number of bytes recieved (side effect)
// Returns:     char*:      char array recieved
// -----------------------------------------------------------------------------
char* receiveArray(int socket_fd, long* len);

// -----------------------------------------------------------------------------
// wait for response and read it into buffer but no length is returned
// Parameters:  int socket_fd:  socket file descriptor from which to read
// Returns:     char*:          char array recieved
// -----------------------------------------------------------------------------
char* receiveString(int socket_fd);

// -----------------------------------------------------------------------------
// connect the client to a server
// Parameters:  char* server_ip:    server address to connect to
// Returns:     int:                socket file descriptor of connection
// -----------------------------------------------------------------------------
int connectToServer(char* server_ip);

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////