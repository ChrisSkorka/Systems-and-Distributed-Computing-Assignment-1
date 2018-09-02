// /////////////////////////////////////////////////////////////////////////////
// Filename:        file.c
// Author:          Christopher Skorka
// Date Created:    30/08/2018
// Description:     File IO, reads and writes messages to and from files
// /////////////////////////////////////////////////////////////////////////////

// INCLUDES ////////////////////////////////////////////////////////////////////
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>

// PRTOTYPES ///////////////////////////////////////////////////////////////////
char* readFile(char* filepath, long* len, int* status);
char* writeFile(char* filename, char* message, long length, int override);
char* getFileName(char* path);

// FUNCTIONS ///////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// read content from specified file
// Parameters:	char* filepath:	file to be red
//				long* len:		stores number of bytes red (side effect)
//				int* status:	stores read status (side effect)
// Returns:		char*:	file content or error message on faliure
// -----------------------------------------------------------------------------
char* readFile(char* filepath, long* len, int* status){
	*status = 0; // status is failed unless changed to success

	// if file isnt available
	if(!fileExists(filepath)){
		char* error = "Error: file doesn't exist";
		*len = strlen(error) + 1;
		return error;
	}
	
	// open file
	char* buffer = 0;
	long length;
	FILE* fp = fopen (filepath, "r");

	// if file opened successfully 
	if(fp){
		// find length of content
		fseek (fp, 0, SEEK_END);
		length = ftell (fp);
		*len = length;
		fseek (fp, 0, SEEK_SET);

		// read from file into buffer
		buffer = malloc(length);
		if(buffer){
			fread(buffer, sizeof(char), length, fp);
		}

		// close file and set status to success
		fclose (fp);
		*status = 1;
		return buffer;

	}else{
		// return error instead
		char* error = "Error: unable to open file";
		*len = strlen(error) + 1;
		return error;
	}
}

// -----------------------------------------------------------------------------
// write char array of given length to specified file
// Parameters:	char* filename: file to be writen to
//				char* message:	message to be written
//				long length:	number of bytes to be written
//				int override:	whether to override an existing file
// Returns:		char*:			NULL or error message on faliour
// -----------------------------------------------------------------------------
char* writeFile(char* filename, char* message, long length, int override){
	// whether the files exists
	int file_exists = fileExists(filename);
	
	// if the file exists and override is false, return an error
	if(!override && file_exists){
		return "Error: file already exists, use -f to override files";
	}

	// create any files necessary
	mkdir(getDirectories(filename), 0777);

    FILE *fp;
    fp = fopen(filename, "w");

	// if file opened successfully
	if(fp){
		fwrite(message, sizeof(char), length, fp);
		fclose(fp);

		// if the file has been overridden, report it
		if(file_exists)
			return "Note: file has been overridden";
		else
			return NULL;
	}else{
		// return error message instead
		return "Error: unable to open file";
	}
}

// -----------------------------------------------------------------------------
// get file name from path
// Parameters:	char* filepath:	file path from which to extract the file name
// Returns:		char*:			string containing the file name
// -----------------------------------------------------------------------------
char* getFileName(char* filepath){
	// return filename
	return basename(filepath);
}

// -----------------------------------------------------------------------------
// get directories from path (remove filename)
// Parameters:	char* path:	file path from which to extract the directories
// Returns:		char*:		string containing the path excliding the file name
// -----------------------------------------------------------------------------
char* getDirectories(char* path){
	// copy path to prevent side effects
	char* local_path = malloc(strlen(path) + 1);
	strcpy(local_path, path);

	// get directories
	return dirname(local_path);
}

// -----------------------------------------------------------------------------
// check if a file already exists
// Parameters:	char* path:	file path to eb checked
// Returns:		int:		1 if file exists 0 otherwise
// -----------------------------------------------------------------------------
int fileExists(char* filepath){
	if(access(filepath, F_OK) == 0)
		return 1; // true: file exists
	else
		return 0; // false file doest not exists or is unavailable
}

// /////////////////////////////////////////////////////////////////////////////
// END OF FILE
// /////////////////////////////////////////////////////////////////////////////