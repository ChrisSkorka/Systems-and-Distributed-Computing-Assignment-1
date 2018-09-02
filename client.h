// /////////////////////////////////////////////////////////////////////////////
// Filename:        client.c
// Author:          Christopher Skorka
// Date Created:    21/08/2018
// Description:     Client side of the remote server program, it queries for
//                  commands, forwards them to the server and prints or saves 
//                  the response
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef CLIENT_H
#define CLIENT_H

// DEFTYPE PROTOTYPES //////////////////////////////////////////////////////////
typedef struct ServerResponse;

// PROTOTYPES //////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// print to the terminal the string provided 40 lines at a time, it waits for 
// enter to be pressed between each block of 40 lines
// Parameters:	char* original_message: the message to be printed
// Returns:		void
// -----------------------------------------------------------------------------
void print(char* original_message);

// -----------------------------------------------------------------------------
// surrounds the message with quotes ("...")
// Parameters:	char* original_message: the message to wrap in quotes
// Returns:		char*:                  quote wrapped message
// -----------------------------------------------------------------------------
char* surroundWithQuotes(char* message);

// -----------------------------------------------------------------------------
// processes the list command
// list [-l] [-f] [pathname] [localfile]
// Parameters:	int argc:           number of arguments provided by the user
//              char** argv:        arguments including command
//              char* server_ip:    the server address to query
// Returns:		ServerResponse:     server responses and flags or error strings
// -----------------------------------------------------------------------------
ServerResponse processList(int argc, char** argv, char* server_ip);

// -----------------------------------------------------------------------------
// processes the get command
// get filepath [-f] [localfile]
// Parameters:	int argc:           number of arguments provided by the user
//              char** argv:        arguments including command
//              char* server_ip:    the server address to query
// Returns:		ServerResponse:     server responses and flags or error strings
// -----------------------------------------------------------------------------
ServerResponse processGet(int argc, char** argv, char* server_ip);

// -----------------------------------------------------------------------------
// processes the put command
// put localfile [-f] [newname]: copy file to server
// Parameters:	int argc:           number of arguments provided by the user
//              char** argv:        arguments including command
//              char* server_ip:    the server address to query
// Returns:		ServerResponse:     server responses and flags or error strings
// -----------------------------------------------------------------------------
ServerResponse processPut(int argc, char** argv, char* server_ip);

// -----------------------------------------------------------------------------
// processes the sys command
// sys
// Parameters:	int argc:           number of arguments provided by the user
//              char** argv:        arguments including command
//              char* server_ip:    the server address to query
// Returns:		ServerResponse:     server responses and flags or error strings
// -----------------------------------------------------------------------------
ServerResponse processSys(int argc, char** argv, char* server_ip);

// -----------------------------------------------------------------------------
// processes the delay command
// delay integer
// Parameters:	int argc:           number of arguments provided by the user
//              char** argv:        arguments including command
//              char* server_ip:    the server address to query
// Returns:		ServerResponse:     server responses and flags or error strings
// -----------------------------------------------------------------------------
ServerResponse processDelay(int argc, char** argv, char* server_ip);

// -----------------------------------------------------------------------------
// processes a command entered by the user
// Parameters:	int argc:               number of arguments provided by the user
//              char** argv:            arguments including command
//              char* server_ip:        the server address which to query
//              char* plain_command:    the plain command used for reference 
//                                      once the response is received
// Returns:		void
// -----------------------------------------------------------------------------
void processCommand(int argc, char** argv, char* server_ip, char* plain_command);

// -----------------------------------------------------------------------------
// main function of client.c 
// Parameters:	
// Returns:		
// -----------------------------------------------------------------------------
void main(int argc, char** argv);

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////