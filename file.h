// /////////////////////////////////////////////////////////////////////////////
// Filename:        file.h
// Author:          Christopher Skorka
// Date Created:    30/08/2018
// Description:     File IO, reads and writes messages to and from files
// /////////////////////////////////////////////////////////////////////////////

// MACROS //////////////////////////////////////////////////////////////////////
#ifndef FILE_H
#define FILE_H

// PROTOTYPES //////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// read content from specified file
// Parameters:	char* filepath:	file to be red
//				long* len:		stores number of bytes red (side effect)
//				int* status:	stores read status (side effect)
// Returns:		char*:	file content or error message on faliure
// -----------------------------------------------------------------------------
char* readFile(char* filepath, long* len, int* status);

// -----------------------------------------------------------------------------
// write char array of given length to specified file
// Parameters:	char* filename: file to be writen to
//				char* message:	message to be written
//				long length:	number of bytes to be written
//				int override:	whether to override an existing file
// Returns:		char*:			NULL or error message on faliour
// -----------------------------------------------------------------------------
char* writeFile(char* filename, char* message, long length, int override);

// -----------------------------------------------------------------------------
// get file name from path
// Parameters:	char* path:	file path from which to extract the file name
// Returns:		char*:		string containing the file name
// -----------------------------------------------------------------------------
char* getFileName(char* path);

// -----------------------------------------------------------------------------
// get directories from path (remove filename)
// Parameters:	char* path:	file path from which to extract the directories
// Returns:		char*:		string containing the path excliding the file name
// -----------------------------------------------------------------------------
char* getDirectories(char* path);

// MACROS //////////////////////////////////////////////////////////////////////
#endif

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////