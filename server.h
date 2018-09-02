// /////////////////////////////////////////////////////////////////////////////
// Filename:        server.h
// Author:          Christopher Skorka
// Date Created:    21/08/2018
// Description:     Server side of the remote progam. It waits for queries,
//                  processes them and responds.
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef SERVER_H
#define SERVER_H

// PROTOTYPES //////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// lets the operating systems shell execute a command and returns its output
// Parameters:	char* shell_command:  the command as a single string 
// Returns:		char*:                shell output or error message on faliure
// -----------------------------------------------------------------------------
char* shell(char* shell_command);

// -----------------------------------------------------------------------------
// main function of server.c, opens and binds a socket, listens for connections
// accept connections and processes connections/requests in new processes
// Parameters:	void
// Returns:		void
// -----------------------------------------------------------------------------
void main();

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////